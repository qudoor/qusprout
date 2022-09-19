import json
from random import randrange

from django.http import HttpResponse, StreamingHttpResponse
from rest_framework.views import APIView

from pyecharts.charts import Bar
from pyecharts.charts import Line
from pyecharts.components import Table
from pyecharts import options as opts
from pyecharts.options import ComponentTitleOpts

from timer.sistask import CSyncTimer
from db.db import DbHandle
from datetime import datetime, timedelta, timezone
import time

# Create your views here.
def response_as_json(data):
    json_str = json.dumps(data)
    response = HttpResponse(
        json_str,
        content_type="application/json",
    )
    response["Access-Control-Allow-Origin"] = "*"
    return response


def json_response(data, code=200):
    data = {
        "code": code,
        "msg": "success",
        "data": data,
    }
    return response_as_json(data)


def json_error(error_string="error", code=500, **kwargs):
    data = {
        "code": code,
        "msg": error_string,
        "data": {}
    }
    data.update(kwargs)
    return response_as_json(data)


JsonResponse = json_response
JsonError = json_error

def countfunc(startTimeStr=None, endTimeStr=None, hostName=None, execType=None, qubitNum=None, interfaceName=None) -> Line:
    x_data = {}
    y_data = {}
    db = DbHandle("a.db")
    resp = db.selectCountData(startTimeStr, endTimeStr, hostName, execType, qubitNum, interfaceName)
    for line in resp:
        if line[5] not in x_data:
            x_data[line[5]] = True

        y_key = str(line[0]) + "_e" + str(line[1]) + "_q" + str(line[2]) + "_" + str(line[3])
        if y_key not in y_data:
            y_data[y_key] = []
        y_data[y_key].append(line[4])

    x_list = []
    for key in x_data:
        x_list.append(key)

    print("x_data:", x_data)
    print("y_data:", y_data)
    a = Line()
    a.add_xaxis(xaxis_data=x_list)
    for key, value in y_data.items():
        a.add_yaxis(
            series_name=key,
            y_axis=value,
            symbol="emptyCircle",
            is_symbol_show=True,
            is_smooth=True,
            label_opts=opts.LabelOpts(is_show=False),
        )

    c = (
        a.set_global_opts(
            title_opts=opts.TitleOpts(
                title='接口调用次数统计表',
                pos_top="top",
                pos_left="center",
            ),
            tooltip_opts=opts.TooltipOpts(
                is_show=True, 
            ),
            xaxis_opts=opts.AxisOpts(
                name='时间', 
                type_="category",
            ),
            yaxis_opts=opts.AxisOpts(
                name='次数',
                type_="value",
                axistick_opts=opts.AxisTickOpts(is_show=True),
                splitline_opts=opts.SplitLineOpts(is_show=True),
            ),
            legend_opts=opts.LegendOpts(
                is_show=True,
                type_='scroll',
                pos_left='center',
                pos_top='bottom',
            ),
        )
        .dump_options_with_quotes()
    )
    return c

def elapsedfunc(startTimeStr=None, endTimeStr=None, hostName=None, execType=None, qubitNum=None, interfaceName=None, elapsed=None) -> Line:
    x_data = {}
    y_data = {}
    db = DbHandle("a.db")
    resp = db.selectElapsedData(startTimeStr, endTimeStr, hostName, execType, qubitNum, interfaceName, elapsed)
    for line in resp:
        if line[6] not in x_data:
            x_data[line[6]] = True

        y_key = str(line[0]) + "_e" + str(line[1]) + "_q" + str(line[2]) + "_" + str(line[3]) + "_el" + str(line[4])
        if y_key not in y_data:
            y_data[y_key] = []
        y_data[y_key].append(line[5])

    x_list = []
    for key in x_data:
        x_list.append(key)

    print("x_data:", x_data)
    print("y_data:", y_data)
    a = Line()
    a.add_xaxis(xaxis_data=x_list)
    for key, value in y_data.items():
        a.add_yaxis(
            series_name=key,
            y_axis=value,
            symbol="emptyCircle",
            is_symbol_show=True,
            is_smooth=True,
            is_clip=False,
            label_opts=opts.LabelOpts(is_show=False),
        )

    c = (
        a.set_global_opts(
            title_opts=opts.TitleOpts(
                title='耗时统计表',
                pos_top="top",
                pos_left="center",
            ),
            tooltip_opts=opts.TooltipOpts(
                is_show=True, 
            ),
            xaxis_opts=opts.AxisOpts(
                name='时间', 
                type_="category",
            ),
            yaxis_opts=opts.AxisOpts(
                name='耗时',
                type_="value",
                axistick_opts=opts.AxisTickOpts(is_show=True),
                splitline_opts=opts.SplitLineOpts(is_show=True),
            ),
            legend_opts=opts.LegendOpts(
                is_show=True,
                type_='scroll',
                pos_left='center',
                pos_top='bottom',
                orient='horizontal',
            ),
        )
        .dump_options_with_quotes()
    )
    return c

def taskfunc(startTimeStr=None, endTimeStr=None, hostName=None, execType=None, qubitNum=None) -> Line:
    x_data = {}
    y_data = {}
    db = DbHandle("a.db")
    resp = db.selectTaskData(startTimeStr, endTimeStr, hostName, execType, qubitNum)
    for line in resp:
        if line[4] not in x_data:
            x_data[line[4]] = True

        y_key = str(line[0]) + "_e" + str(line[1]) + "_q" + str(line[2]) 
        if y_key not in y_data:
            y_data[y_key] = []
        y_data[y_key].append(line[3])

    x_list = []
    for key in x_data:
        x_list.append(key)

    print("x_data:", x_data)
    print("y_data:", y_data)
    a = Line()
    a.add_xaxis(xaxis_data=x_list)
    for key, value in y_data.items():
        a.add_yaxis(
            series_name=key,
            y_axis=value,
            symbol="emptyCircle",
            is_symbol_show=True,
            is_smooth=True,
            label_opts=opts.LabelOpts(is_show=False),
        )

    c = (
        a.set_global_opts(
            title_opts=opts.TitleOpts(
                title='总任务统计表',
                pos_top="top",
                pos_left="center",
            ),
            tooltip_opts=opts.TooltipOpts(
                is_show=True, 
            ),
            xaxis_opts=opts.AxisOpts(
                name='时间', 
                type_="category",
            ),
            yaxis_opts=opts.AxisOpts(
                name='任务数',
                type_="value",
                axistick_opts=opts.AxisTickOpts(is_show=True),
                splitline_opts=opts.SplitLineOpts(is_show=True),
            ),
            legend_opts=opts.LegendOpts(
                is_show=True,
                type_='scroll',
                pos_left='center',
                pos_top='bottom',
                orient='horizontal',
            ),
        )
        .dump_options_with_quotes()
    )
    return c

def codefunc(startTimeStr=None, endTimeStr=None, hostName=None, execType=None, qubitNum=None, interfaceName=None, code=None) -> Line:
    x_data = {}
    y_data = {}
    db = DbHandle("a.db")
    resp = db.selectCodeData(startTimeStr, endTimeStr, hostName, execType, qubitNum, interfaceName, code)
    for line in resp:
        if line[6] not in x_data:
            x_data[line[6]] = True

        y_key = str(line[0]) + "_e" + str(line[1]) + "_q" + str(line[2]) + "_" + str(line[3]) + "_cd" + str(line[4])
        if y_key not in y_data:
            y_data[y_key] = []
        y_data[y_key].append(line[5])

    x_list = []
    for key in x_data:
        x_list.append(key)

    print("x_data:", x_data)
    print("y_data:", y_data)
    a = Line()
    a.add_xaxis(xaxis_data=x_list)
    for key, value in y_data.items():
        a.add_yaxis(
            series_name=key,
            y_axis=value,
            symbol="emptyCircle",
            is_symbol_show=True,
            is_smooth=True,
            label_opts=opts.LabelOpts(is_show=False),
        )

    c = (
        a.set_global_opts(
            title_opts=opts.TitleOpts(
                title='错误码统计表',
                pos_top="top",
                pos_left="center",
            ),
            tooltip_opts=opts.TooltipOpts(
                is_show=True, 
            ),
            xaxis_opts=opts.AxisOpts(
                name='时间', 
                type_="category",
            ),
            yaxis_opts=opts.AxisOpts(
                name='次数',
                type_="value",
                axistick_opts=opts.AxisTickOpts(is_show=True),
                splitline_opts=opts.SplitLineOpts(is_show=True),
            ),
            legend_opts=opts.LegendOpts(
                is_show=True,
                type_='scroll',
                pos_left='center',
                pos_top='bottom',
            ),
        )
        .dump_options_with_quotes()
    )
    return c

def currfunc(startTimeStr=None, endTimeStr=None, hostName=None, execType=None, qubitNum=None) -> Line:
    x_data = {}
    y_data = {}
    db = DbHandle("a.db")
    resp = db.selectCurrTaskData(startTimeStr, endTimeStr, hostName, execType, qubitNum)
    for line in resp:
        if line[5] not in x_data:
            x_data[line[5]] = True

        y_key = str(line[0]) + "_e" + str(line[1]) + "_q" + str(line[2])
        y0_key = y_key + "_curr"
        if y0_key not in y_data:
            y_data[y0_key] = []
        y_data[y0_key].append(line[3])
        
        y1_key = y_key + "_timeout"
        if y1_key not in y_data:
            y_data[y1_key] = []
        y_data[y1_key].append(line[4])

    x_list = []
    for key in x_data:
        x_list.append(key)

    print("x_data:", x_data)
    print("y_data:", y_data)
    a = Line()
    a.add_xaxis(xaxis_data=x_list)
    for key, value in y_data.items():
        a.add_yaxis(
            series_name=key,
            y_axis=value,
            symbol="emptyCircle",
            is_symbol_show=True,
            is_smooth=True,
            label_opts=opts.LabelOpts(is_show=False),
        )

    c = (
        a.set_global_opts(
            title_opts=opts.TitleOpts(
                title='当前任务统计表',
                pos_top="top",
                pos_left="center",
            ),
            tooltip_opts=opts.TooltipOpts(
                is_show=True, 
            ),
            xaxis_opts=opts.AxisOpts(
                name='时间', 
                type_="category",
            ),
            yaxis_opts=opts.AxisOpts(
                name='当前任务数/超时任务数',
                type_="value",
                axistick_opts=opts.AxisTickOpts(is_show=True),
                splitline_opts=opts.SplitLineOpts(is_show=True),
            ),
            legend_opts=opts.LegendOpts(
                is_show=True,
                type_='scroll',
                pos_left='center',
                pos_top='bottom',
            ),
        )
        .dump_options_with_quotes()
    )
    return c

def statusfunc(startTimeStr=None, endTimeStr=None, hostName=None) -> Line:
    x_data = {}
    y_data = {}
    db = DbHandle("a.db")
    resp = db.selectStatusData(startTimeStr, endTimeStr, hostName)
    for line in resp:
        if line[7] not in x_data:
            x_data[line[7]] = True

        y_key = str(line[0])
        y0_key = y_key + "_usecpumem"
        if y0_key not in y_data:
            y_data[y0_key] = []
        y_data[y0_key].append(line[3])

    x_list = []
    for key in x_data:
        x_list.append(key)

    a = Line()
    a.add_xaxis(xaxis_data=x_list)
    for key, value in y_data.items():
        a.add_yaxis(
            series_name=key,
            y_axis=value,
            symbol="emptyCircle",
            is_symbol_show=True,
            is_smooth=True,
            label_opts=opts.LabelOpts(is_show=False),
        )

    c = (
        a.set_global_opts(
            title_opts=opts.TitleOpts(
                title='当前资源变化统计表',
                pos_top="top",
                pos_left="center",
            ),
            tooltip_opts=opts.TooltipOpts(
                is_show=True, 
            ),
            xaxis_opts=opts.AxisOpts(
                name='时间', 
                type_="category",
            ),
            yaxis_opts=opts.AxisOpts(
                name='MB',
                type_="value",
                axistick_opts=opts.AxisTickOpts(is_show=True),
                splitline_opts=opts.SplitLineOpts(is_show=True),
            ),
            legend_opts=opts.LegendOpts(
                is_show=True,
                type_='scroll',
                pos_left='center',
                pos_top='bottom',
            ),
        )
        .dump_options_with_quotes()
    )
    return c

def headtabfunc(startTimeStr=None, endTimeStr=None, hostName=None) -> Table:
    headers = ["hostName", "allCpuNum", "allMemory", "gpuType", "allGpuMemory"]
    rows = []
    db = DbHandle("a.db")
    resp = db.selectnewdata(startTimeStr, endTimeStr, hostName)
    for line in resp:
        data = {}
        data[headers[0]] = line[0]
        data[headers[1]] = line[1]
        data[headers[2]] = line[2]
        data[headers[3]] = line[3]
        data[headers[4]] = line[4]
        rows.append(data)

    print("headers:", headers)
    print("rows:", rows)
    return json.dumps(rows)

def Tagfunc(startTimeStr=None, endTimeStr=None):
    db = DbHandle("a.db")
    resp = db.selectTagList(startTimeStr, endTimeStr)
    return resp

def hostinfofunc(hostname=None):
    db = DbHandle("a.db")
    resp = db.selectHostInfo(hostname)
    return resp

class CountView(APIView):
    def get(self, request, *args, **kwargs):
        hostname = request.query_params.get('hostname')
        if hostname == 'All':
            hostname = None
        exectype = request.query_params.get('exectype')
        if exectype == 'All':
            exectype = None
        qubit = request.query_params.get('qubit')
        if qubit == 'All':
            qubit = None
        intername = request.query_params.get('intername')
        if intername == 'All':
            intername = None
        recent = request.query_params.get('recent')
        if recent == 'All':
            recent = None
        currtime = time.time() - int(recent)*24*60*60
        beginTime = datetime.fromtimestamp(currtime, timezone(timedelta(hours=8)))
        beginTime = beginTime.strftime("%Y-%m-%d") + " 00:00:00"
        return JsonResponse(json.loads(countfunc(beginTime, "", hostname, exectype, qubit, intername)))

class ElapsedView(APIView):
    def get(self, request, *args, **kwargs):
        hostname = request.query_params.get('hostname')
        if hostname == 'All':
            hostname = None
        exectype = request.query_params.get('exectype')
        if exectype == 'All':
            exectype = None
        qubit = request.query_params.get('qubit')
        if qubit == 'All':
            qubit = None
        intername = request.query_params.get('intername')
        if intername == 'All':
            intername = None
        elapsed = request.query_params.get('elapsed')
        if elapsed == 'All':
            elapsed = None
        recent = request.query_params.get('recent')
        if recent == 'All':
            recent = None
        currtime = time.time() - int(recent)*24*60*60
        beginTime = datetime.fromtimestamp(currtime, timezone(timedelta(hours=8)))
        beginTime = beginTime.strftime("%Y-%m-%d") + " 00:00:00"
        return JsonResponse(json.loads(elapsedfunc(beginTime, "", hostname, exectype, qubit, intername, elapsed)))

class TaskView(APIView):
    def get(self, request, *args, **kwargs):
        hostname = request.query_params.get('hostname')
        if hostname == 'All':
            hostname = None
        exectype = request.query_params.get('exectype')
        if exectype == 'All':
            exectype = None
        qubit = request.query_params.get('qubit')
        if qubit == 'All':
            qubit = None
        recent = request.query_params.get('recent')
        if recent == 'All':
            recent = None
        currtime = time.time() - int(recent)*24*60*60
        beginTime = datetime.fromtimestamp(currtime, timezone(timedelta(hours=8)))
        beginTime = beginTime.strftime("%Y-%m-%d") + " 00:00:00"
        return JsonResponse(json.loads(taskfunc(beginTime, "", hostname, exectype, qubit)))

class CodeView(APIView):
    def get(self, request, *args, **kwargs):
        hostname = request.query_params.get('hostname')
        if hostname == 'All':
            hostname = None
        exectype = request.query_params.get('exectype')
        if exectype == 'All':
            exectype = None
        qubit = request.query_params.get('qubit')
        if qubit == 'All':
            qubit = None
        intername = request.query_params.get('intername')
        if intername == 'All':
            intername = None
        code = request.query_params.get('code')
        if code == 'All':
            code = None
        recent = request.query_params.get('recent')
        if recent == 'All':
            recent = None
        currtime = time.time() - int(recent)*24*60*60
        beginTime = datetime.fromtimestamp(currtime, timezone(timedelta(hours=8)))
        beginTime = beginTime.strftime("%Y-%m-%d") + " 00:00:00"
        return JsonResponse(json.loads(codefunc(beginTime, "", hostname, exectype, qubit, intername, code)))

class CurrView(APIView):
    def get(self, request, *args, **kwargs):
        hostname = request.query_params.get('hostname')
        if hostname == 'All':
            hostname = None
        exectype = request.query_params.get('exectype')
        if exectype == 'All':
            exectype = None
        qubit = request.query_params.get('qubit')
        if qubit == 'All':
            qubit = None
        recent = request.query_params.get('recent')
        if recent == 'All':
            recent = None
        currtime = time.time() - int(recent)*24*60*60
        beginTime = datetime.fromtimestamp(currtime, timezone(timedelta(hours=8)))
        beginTime = beginTime.strftime("%Y-%m-%d") + " 00:00:00"
        return JsonResponse(json.loads(currfunc(beginTime, "", hostname, exectype, qubit)))

class StatusView(APIView):
    def get(self, request, *args, **kwargs):
        hostname = request.query_params.get('hostname')
        if hostname == 'All':
            hostname = None
        recent = request.query_params.get('recent')
        if recent == 'All':
            recent = None
        currtime = time.time() - int(recent)*24*60*60
        beginTime = datetime.fromtimestamp(currtime, timezone(timedelta(hours=8)))
        beginTime = beginTime.strftime("%Y-%m-%d") + " 00:00:00"
        return JsonResponse(json.loads(statusfunc(beginTime, "", hostname)))

class HostInfoView(APIView):
    def get(self, request, *args, **kwargs):
        hostname = request.query_params.get('hostname')
        if hostname == 'All':
            hostname = None
        return JsonResponse(hostinfofunc(hostname))

class HeadtabView(APIView):
    def get(self, request, *args, **kwargs):
        hostname = request.query_params.get('hostname')
        if hostname == 'All':
            hostname = None
        recent = request.query_params.get('recent')
        if recent == 'All':
            recent = None
        currtime = time.time() - int(recent)*24*60*60
        beginTime = datetime.fromtimestamp(currtime, timezone(timedelta(hours=8)))
        beginTime = beginTime.strftime("%Y-%m-%d") + " 00:00:00"
        return JsonResponse(json.loads(headtabfunc(beginTime, "", hostname)))

class TagView(APIView):
    def get(self, request, *args, **kwargs):
        return JsonResponse(Tagfunc("", ""))

class IndexView(APIView):
    def get(self, request, *args, **kwargs):
        return HttpResponse(content=open("./templates/index.html", 'r', encoding='utf-8').read())

timer = CSyncTimer()
timer.start()