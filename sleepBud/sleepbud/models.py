from django.db import models
from django.contrib.auth.models import User

# Create your models here.


class BodyStatus(models.Model):
    user = models.ForeignKey(
        User,
        on_delete=models.CASCADE,
    )
    Snoring = models.DecimalField(max_digits=1, decimal_places=0, default=0)
    BMI_30 = models.DecimalField(max_digits=1, decimal_places=0, default=0)
    alcoholism = models.DecimalField(max_digits=1, decimal_places=0, default=0)
    day_sleepy = models.DecimalField(max_digits=1, decimal_places=0, default=0)
    Irritability = models.DecimalField(
        max_digits=1, decimal_places=0, default=0)
    age = models.DecimalField(max_digits=4, decimal_places=1, default=0)
    inputdate = models.DateTimeField(auto_now_add=True, blank=True)


class data(models.Model):

    user = models.ForeignKey(
        User,
        on_delete=models.CASCADE,
    )
    temp = models.DecimalField(max_digits=4, decimal_places=2)
    snore = models.DecimalField(max_digits=4, decimal_places=2)
    body_position = models.DecimalField(max_digits=2, decimal_places=0)
    testdate = models.DateTimeField(auto_now_add=True, blank=True)
