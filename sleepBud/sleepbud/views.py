from __future__ import unicode_literals
from django.shortcuts import render
from django.http import HttpResponse, HttpResponseRedirect
from django.contrib.auth import authenticate
import threading
import time
import serial
from .models import *
# from .services import *
import json


port = '/dev/cu.usbmodem141301'
ard = serial.Serial(port, 115200, timeout=5)


# Create your views here.
def index(request):
    if 'user_id' in request.session:
        user = authenticate(id=request.session['user_id'])
        if user is not None:
            return HttpResponseRedirect('/sleepbud/input/')

    if request.method == 'POST':
        email = request.POST['email']
        password = request.POST['password']
        user = authenticate(username=email, password=password)
        if user is not None:
            request.session["user_id"] = user.id
            return HttpResponseRedirect('/sleepbud/input/')
        else:
            return HttpResponse("The email and password pair does not exist.")
    return render(request, "index.html")


def register(request):
    if request.method == 'POST':
        email = request.POST['email']
        password = request.POST['password']
        firstname = request.POST['firstname']
        lastname = request.POST['lastname']
        user = User.objects.create_user(
            username=email,
            password=password,
            email=email,
            first_name=firstname,
            last_name=lastname
        )
        user.save()
        return HttpResponseRedirect('/sleepbud/')
    return render(request, "register.html")


def input(request):
    user_id = request.session['user_id']
    user = User.objects.get(pk=user_id)
    if request.method == 'POST':
        new_input_satuts = BodyStatus(
            user=user,
            Snoring=request.POST['Snoring'],
            BMI_30=request.POST['BMI_30'],
            alcoholism=request.POST['alcoholism'],
            day_sleepy=request.POST['day_sleepy'],
            Irritability=request.POST['Irritability'],
            age=request.POST['age'],
        )
        new_input_satuts.save()
        body_status = BodyStatus.objects.filter(
            user=user).order_by('-id')[0]
        context = {
            "user": user,
            "tag": "input",
            "status": body_status,
            "message": "Body data helping diagnose saved successfully!",
        }
        return render(request, "input.html", context)
    if BodyStatus.objects.filter(user=user).exists():
        body_status = BodyStatus.objects.filter(
            user=user).order_by('-id')[0]
        context = {
            "user": user,
            "tag": "input",
            "status": body_status,
        }
        return render(request, "input.html", context)
    context = {
        "user": user,
        "tag": "input",
    }
    return render(request, "input.html", context)


def arduino(request):
    user_id = request.session['user_id']
    user = User.objects.get(pk=user_id)

    context = {
        "user": user,
        "data": data.objects.filter(user=user).order_by('-testdate')[0],
        "BodyStatus": BodyStatus.objects.filter(user=user).order_by('-inputdate')[0],

    }
    return render(request, "test.html", context)


def printit():
    threading.Timer(5.0, printit).start()
    msg = str(ard.readline(), encoding="utf-8").strip("\n").strip("\r")
    print("Message from arduino: ")
    print(msg.split("_"))
    if len(msg) > 5:
        data_1 = msg.split("_")[0]
        data_2 = msg.split("_")[1]
        data_3 = msg.split("_")[2]

        new_test_satuts = data(
            user_id=1,
            temp=float(data_1),
            snore=float(data_2),
            body_position=int(data_3)
        )
        new_test_satuts.save()


printit()
