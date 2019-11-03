from django.contrib import admin
from django.urls import path
from django.conf.urls import url, include
from sleepbud import views

urlpatterns = [
    url(r'^$', views.index, name='index'),
    url(r'^input/$', views.input, name='input'),
    url(r'^arduino/$', views.arduino, name='arduino'),

]
