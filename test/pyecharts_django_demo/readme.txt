cd /root/code/qubox/src/qusprout/idl
thrift --gen py ecode.thrift
thrift --gen py:package_prefix='qusprout.' resource.thrift
thrift --gen py:package_prefix='qusprout.' qusproutdata.thrift
thrift --gen py:package_prefix='qusprout.' statistics.thrift
cd /root/code/qubox/src/qusprout/idl/gen-py
mv * /root/pydir/pyecharts_django_demo/qusprout/

python3 manage.py runserver 0.0.0.0:8000


sqlite3 
.open /root/pydir/pyecharts_django_demo/a.db
SELECT name FROM sqlite_master WHERE type='table' order by name;
.ext