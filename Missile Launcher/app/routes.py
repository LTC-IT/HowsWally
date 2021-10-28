from app import app
import random
from flask import render_template, redirect, flash
from app.forms import LoginForm
from flask_login import logout_user, current_user
from flask_login import login_required, login_user
from flask import url_for
from app.models import User
from app import db
from app.forms import RegistrationForm
from flask_login import LoginManager, logout_user, login_required
import os  # Imports the miscellaneous operating system interfaces library.
import sys  # Import the system specific parameters and functions library.
import time  # Imports the time library
import usb.core  # Imports usb.core which allows the use of the usb missile launcher.
import smbus  # Imports necessary function for the i2c module
import time  # Imports necessary function for the i2c module

# Define some device parameters
I2C_ADDR = 0x3f  # I2C device address
LCD_WIDTH = 16  # Maximum characters per line

# Define some device constants
LCD_CHR = 1  # Mode - Sending data
LCD_CMD = 0  # Mode - Sending command

LCD_LINE_1 = 0x80  # LCD RAM address for the 1st line
LCD_LINE_2 = 0xC0  # LCD RAM address for the 2nd line
LCD_LINE_3 = 0x94  # LCD RAM address for the 3rd line
LCD_LINE_4 = 0xD4  # LCD RAM address for the 4th line

LCD_BACKLIGHT = 0x08  # On
# LCD_BACKLIGHT = 0x00  # Off

ENABLE = 0b00000100  # Enable bit

# Timing constants
E_PULSE = 0.0005
E_DELAY = 0.0005

# Open I2C interface
# bus = smbus.SMBus(0)  # Rev 1 Pi uses 0
bus = smbus.SMBus(1)  # Rev 2 Pi uses 1

def lcd_init():
    # Initialise display
    lcd_byte(0x33, LCD_CMD)  # 110011 Initialise
    lcd_byte(0x32, LCD_CMD)  # 110010 Initialise
    lcd_byte(0x06, LCD_CMD)  # 000110 Cursor move direction
    lcd_byte(0x0C, LCD_CMD)  # 001100 Display On,Cursor Off, Blink Off
    lcd_byte(0x28, LCD_CMD)  # 101000 Data length, number of lines, font size
    lcd_byte(0x01, LCD_CMD)  # 000001 Clear display
    time.sleep(E_DELAY)

def lcd_byte(bits, mode):
    # Send byte to data pins
    # bits = the data
    # mode = 1 for data
    #        0 for command

    bits_high = mode | (bits & 0xF0) | LCD_BACKLIGHT
    bits_low = mode | ((bits << 4) & 0xF0) | LCD_BACKLIGHT

    # High bits
    bus.write_byte(I2C_ADDR, bits_high)
    lcd_toggle_enable(bits_high)

    # Low bits
    bus.write_byte(I2C_ADDR, bits_low)
    lcd_toggle_enable(bits_low)

def lcd_toggle_enable(bits):
    # Toggle enable
    time.sleep(E_DELAY)
    bus.write_byte(I2C_ADDR, (bits | ENABLE))
    time.sleep(E_PULSE)
    bus.write_byte(I2C_ADDR, (bits & ~ENABLE))
    time.sleep(E_DELAY)

def lcd_string(message, line):
    # Send string to display

    message = message.ljust(LCD_WIDTH, " ")

    lcd_byte(line, LCD_CMD)

    for i in range(LCD_WIDTH):
        lcd_byte(ord(message[i]), LCD_CHR)

def main():
    # Main program block

    # Initialise display
    lcd_init()

    while True:
        # Send some test
        lcd_string("samNATJK2       ", LCD_LINE_1)

@app.route('/')
# """This is the login page for when the user runs
# missile launcher code"""
@app.route('/index')
def main_page():
    return redirect(url_for('login'))


@app.route('/login', methods=['GET', 'POST'])
# """This will check whether the user inputs the correct
# username and password, if correct the user is granted
# access to fire the missile launcher"""
def login():
    if current_user.is_authenticated:
        return redirect(url_for('fire'))
    form = LoginForm()
    if form.validate_on_submit():
        user = User.query.filter_by(username=form.username.data).first()
        if user is None or not user.check_password(form.password.data):
            # flash('Invalid username or password')
            return redirect(url_for('fire'))
        login_user(user, remember=form.remember_me.data)
        return redirect(url_for('fire'))
    return render_template('login.html', title='Sign In', form=form)

#user: root password#
@app.route('/fire')
# """This fires the missile launcher"""
def fire():

    return render_template('fire.html')


@app.route('/logout')
# """This redirects the user to the main page if
# they wish to logout"""
def logout():
    logout_user()
    return redirect(url_for('main_page'))


@app.route('/tgwjkdfhaasdnbfmasdfvgkasd')  # This will run if the letters included here are put in the url.
@login_required
def fire_missile():  # This is the fire function controls the missile fire
    """This function is what allows the missile to fire,
    it also detects the missile launcher from the USB."""
    dev = usb.core.find(idVendor=0x2123, idProduct=0x1010)  # Finds the missile launcher via usb.
    # #TR-DEF-ML ^
    #dev = usb.core.find(idVendor=0x0a81, idProduct=0x0701)  # Finds the missile launcher via usb.
    if dev is None:  # If it can't find the missile launcher.
        raise ValueError('Launcher not found.')  # This will be printed if the missile launcher cannot be found.
    if dev.is_kernel_driver_active(
            0) is True:  # Determines whether or not there is a Kernel driver associated with the missile launcher.
        dev.detach_kernel_driver(0)  # If there is a Kernal driver associated with the missile launcher it detaches it.
        dev.set_configuration()  # The code will then configure the USB missile launcher.

        # The 0x10 is the command which fires. Replace this with other commands as necessary
        #https://github.com/codedance/Retaliation/blob/master/retaliation.py
    #dev.ctrl_transfer(0x21, 0x09, 0x0200, 0, [0x02])
    dev.ctrl_transfer(0x21, 0x09, 0, 0,
                      [0x02, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])  # This is what allows the missile to fire.
    return render_template('fire.html')
#The motor appeears not to be working, up,down,left and right commonads aren't working

if __name__ == '__main__':
    APP.run(port=80)
