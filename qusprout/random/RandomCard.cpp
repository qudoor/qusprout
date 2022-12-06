/****************************************************************************
*
* This code is licensed under the Apache License, Version 2.0. You may
* obtain a copy of this license in the LICENSE.txt file in the root directory
* of this source tree or at http://www.apache.org/licenses/LICENSE-2.0.
*
* Any modifications or derivative works of this code must retain this
* copyright notice, and modified files need to carry a notice indicating
* that they have been altered from the originals.
*
****************************************************************************/

#include "RandomCard.h"
#include "comm/Base.h"
#include "common/base64.h"
#include "common/qulog.h"
#ifdef QRNG_PCIE
    #include "qrng_pcie.h"
#endif

CRandomCardManager::CRandomCardManager()
{

}

CRandomCardManager::~CRandomCardManager()
{

}

//获取随机数卡的信息
void CRandomCardManager::getRandomCardInfo(GetRandomCardInfoResp& resp, const GetRandomCardInfoReq& req)
{
#ifdef QRNG_PCIE
    uint32_t count = 0;
    //获取设备数量
    int ret = qrng_pcie_count(&count);
    if (ret != 0)
    {
        LOG(ERROR) << "qrng_pcie_count failed(ret:" << ret << ").";
        setBase(resp.base, ErrCode::type::COM_NOT_EXIST, "not find random card.");
        return;
    }

    //获取驱动版本号
    uint32_t driverversion = 0;
    ret = qrng_pcie_get_driver_version(&driverversion);
    if (ret != 0)
    {
        LOG(ERROR) << "qrng_pcie_get_driver_version failed(ret:" << ret << ").";
        setBase(resp.base, ErrCode::type::COM_NOT_EXIST, "not find random card dirve.");
        return;
    }

    //获取接口库版本号
    uint32_t libraryversion = 0;
    ret = qrng_pcie_get_library_version(&libraryversion);
    if (ret != 0)
    {
        LOG(ERROR) << "qrng_pcie_get_library_version failed(ret:" << ret << ").";
        setBase(resp.base, ErrCode::type::COM_NOT_EXIST, "not find random card library.");
        return;
    }
    resp.__set_count(count);
    resp.__set_driver_version(driverversion);
    resp.__set_library_version(libraryversion);
    
    for (uint32_t i = 0; i < count; ++i)
    {
        uint32_t deviceindex = i;
        //获取设备随机数输出方式
        uint8_t mode = 0;
        ret = qrng_pcie_get_output(deviceindex, &mode);
        if (ret != 0)
        {
            LOG(ERROR) << "qrng_pcie_get_output failed(ret:" << ret << ").";
            setBase(resp.base, ErrCode::type::COM_OTHRE, "call random card mode failed.");
            return;
        }

        //获取设备激光器温度
        float ldtemp = 0;
        ret = qrng_pcie_get_ld_temp(deviceindex, &ldtemp);
        if (ret != 0)
        {
            LOG(ERROR) << "qrng_pcie_get_ld_temp failed(ret:" << ret << ").";
            setBase(resp.base, ErrCode::type::COM_OTHRE, "call random card laser temperature failed.");
            return;
        }

        //获取设备电路板温度
        float bdtemp = 0;
        ret = qrng_pcie_get_bd_temp(deviceindex, &bdtemp);
        if (ret != 0)
        {
            LOG(ERROR) << "qrng_pcie_get_bd_temp failed(ret:" << ret << ").";
            setBase(resp.base, ErrCode::type::COM_OTHRE, "call random card circuit temperature failed.");
            return;
        }

        //获取设备状态
        uint32_t state = 0;
        ret = qrng_pcie_get_state(deviceindex, &state);
        if (ret != 0)
        {
            LOG(ERROR) << "qrng_pcie_get_state failed(ret:" << ret << ").";
            setBase(resp.base, ErrCode::type::COM_OTHRE, "call random card status failed.");
            return;
        }

        RandomCardInfo cards;
        cards.__set_device_index(deviceindex);
        cards.__set_mode(mode);
        cards.__set_ld_temp(ldtemp);
        cards.__set_bd_temp(bdtemp);
        for (uint32_t j = 0; j < 9; ++j)
        {
            RandomCardStateType::type cardtype;
            int value = (int)((state >> j) & 0x00000001);
            switch(j)
            {
            case 0:
                cardtype = RandomCardStateType::type::RANDOM_MC_S0;
                break;
            case 1:
                cardtype = RandomCardStateType::type::RANDOM_MC_S1;
                break;
            case 2:
                cardtype = RandomCardStateType::type::RANDOM_EEPROM_S;
                break;
            case 3:
                cardtype = RandomCardStateType::type::RANDOM_PAR_VALUE_S;
                break;
            case 4:
                cardtype = RandomCardStateType::type::RANDOM_EEPROM_CHECK_S;
                break;
            case 5:
                cardtype = RandomCardStateType::type::RANDOM_EEPROM_RW_S;
                break;
            case 6:
                cardtype = RandomCardStateType::type::RANDOM_LD_TEMP_S;
                break;
            case 7:
                cardtype = RandomCardStateType::type::RANDOM_BD_TEMP_S;
                break;
            case 8:
                cardtype = RandomCardStateType::type::RANDOM_LINK_S;
                break;
            }
            cards.states.insert(std::pair<RandomCardStateType::type, int>(cardtype, value));
        }
        resp.cards.push_back(cards);
    }

    setBase(resp.base, ErrCode::type::COM_SUCCESS);
#else 
    setBase(resp.base, ErrCode::type::COM_NOT_EXIST, "not find random card.");
#endif
}

//设置随机数卡
void CRandomCardManager::setRandomCard(SetRandomCardResp& resp, const SetRandomCardReq& req)
{
    auto deviceindex = req.device_index;
    if (!req.__isset.mode && !req.__isset.reset && req.reset)
    {
        LOG(ERROR) << "mode or reset is not invaild(deviceindex:" << deviceindex << ").";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM, "required param is must fill in of mode or reset.");
        return;
    }

#ifdef QRNG_PCIE
    //获取设备数量
    unsigned int count = 0;
    int ret = qrng_pcie_count(&count);
    if (ret != 0)
    {
        LOG(ERROR) << "qrng_pcie_count failed(ret:" << ret << ").";
        setBase(resp.base, ErrCode::type::COM_NOT_EXIST, "not find random card.");
        return;
    }
    
    if (deviceindex < 0 || (unsigned int)deviceindex >= count)
    {
        LOG(ERROR) << "device_index is not invaild(deviceindex:" << deviceindex << ",count:" << count << ").";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM, "device_index is over vaild range.");
        return;
    }

    if (req.__isset.mode)
    {
        auto mode = req.mode;
        if (mode < 0 || mode > 3)
        {
            LOG(ERROR) << "mode is not invaild(deviceindex:" << deviceindex << ",mode:" << mode << ").";
            setBase(resp.base, ErrCode::type::COM_INVALID_PARAM, "mode is over vaild range.");
            return;
        }

        //设置设备随机数输出方式
        ret = qrng_pcie_ctrl_output(deviceindex, mode);
        if (ret != 0)
        {
            LOG(ERROR) << "qrng_pcie_ctrl_output failed(ret:" << ret << ").";
            setBase(resp.base, ErrCode::type::COM_OTHRE, "set random card mode failed.");
            return;
        }
    }
    else
    {
        //复位设备
        ret = qrng_pcie_reset(deviceindex);
        if (ret != 0)
        {
            LOG(ERROR) << "qrng_pcie_reset failed(ret:" << ret << ").";
            setBase(resp.base, ErrCode::type::COM_OTHRE, "reset random card failed.");
            return;
        }
    }

    setBase(resp.base, ErrCode::type::COM_SUCCESS);
#else 
    setBase(resp.base, ErrCode::type::COM_NOT_EXIST, "not find random card.");
#endif
}

//获取随机数
void CRandomCardManager::getRandom(GetRandomResp& resp, const GetRandomReq& req)
{
    auto randomlength = req.random_length;
    auto randomnum = req.random_num;
    if (randomlength <= 0 || randomnum <= 0)
    {
        LOG(ERROR) << "param is not invaild(randomlength:" << randomlength << ",randomnum:" << randomnum << ").";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM, "invaild param of random_length or random_num.");
        return;
    }

#ifdef QRNG_PCIE
    //获取设备数量
    unsigned int count = 0;
    int ret = qrng_pcie_count(&count);
    if (ret != 0)
    {
        LOG(ERROR) << "qrng_pcie_count failed(ret:" << ret << ").";
        setBase(resp.base, ErrCode::type::COM_NOT_EXIST, "not find random card.");
        return;
    }
    
    int deviceindex = -1;
    if (req.__isset.device_index)
    {
        deviceindex = req.device_index;
        if (deviceindex < 0 || (unsigned int)deviceindex >= count)
        {
            LOG(ERROR) << "device_index is not invaild(deviceindex:" << deviceindex << ",count:" << count << ").";
            setBase(resp.base, ErrCode::type::COM_INVALID_PARAM, "device_index is over vaild range.");
            return;
        }
    }

    std::unique_ptr<unsigned char []> data(new unsigned char[randomlength+1]());
    for (int i = 0; i < randomnum; ++i)
    {
        if (deviceindex < 0)
        {
            auto index = m_index++;
            deviceindex = index % count;
        }

        memset(data.get(), '\0', randomlength+1);

        //从设备读取随机数
        ret = qrng_pcie_read_random_by_device(deviceindex, data.get(), randomlength);
        if (ret != 0)
        {
            LOG(ERROR) << "qrng_pcie_read_random_by_device failed(ret:" << ret << ").";
            setBase(resp.base, ErrCode::type::COM_OTHRE, "read random data failed.");
            return;
        }

        std::string hexstr;
        char buf[3] = "";
        for (int i = 0; i < randomlength; i++)
        {
            memset(buf, '\0', 3);
            std::snprintf(buf, 3,"%02x",data[i]);
            hexstr += buf;
        }
        LOG(INFO) << "qrng_pcie_read_random_by_device success(hexstr:" << hexstr << ").";

        std::string temp((char*)data.get(), randomlength);
        auto random = encodeBase64(temp);
        resp.randoms.push_back(random);
    }
    
    setBase(resp.base, ErrCode::type::COM_SUCCESS);
#else 
    setBase(resp.base, ErrCode::type::COM_NOT_EXIST, "not find random card.");
#endif
}
