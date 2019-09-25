from django.urls import path

from login import views

urlpatterns = [
    path('', views.index, name='index'),
    path('login/', views.login),
    path('register/', views.register),
    path('logout/', views.logout),
]