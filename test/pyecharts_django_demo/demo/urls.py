from django.urls import re_path as url
from . import views

urlpatterns = [
    url(r'^count/$', views.CountView.as_view(), name='demo'),
    url(r'^elapsed/$', views.ElapsedView.as_view(), name='demo'),
    url(r'^task/$', views.TaskView.as_view(), name='demo'),
    url(r'^code/$', views.CodeView.as_view(), name='demo'),
    url(r'^curr/$', views.CurrView.as_view(), name='demo'),
    url(r'^tag/$', views.TagView.as_view(), name='demo'),
    url(r'^status/$', views.StatusView.as_view(), name='demo'),
    url(r'^headtab/$', views.HeadtabView.as_view(), name='demo'),
    url(r'^index/$', views.IndexView.as_view(), name='demo'),
    url(r'^hostinfo/$', views.HostInfoView.as_view(), name='demo'),
]