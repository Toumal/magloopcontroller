# magloopcontroller
A simple DC motor based remote tuner for magnetic loop antennas.

## Parts
* 2 x Arduino Nano (or compatible, I used a mega because I had it around)
* 2 x NRF24L01 2.4GHz Wireless Transceiver Module
* 1 x L298N Dual H-Bridge Motor Driver Board (or similar)
* 3 x LED
* 2 x Momentary Pushbuttons closing type
* 4 x 10k resistor
* 3 x 300 Ohms resistor
* 1 x Variable capacitor with DC motor for your magnetic loop (Check ebay)

## Rationale

I'm not going into details on how to construct a magnetic loop antenna. There's plenty of resources with info on how to do that, including online calculators that will give you all the needed info on how long and thick the loop should be, etc.
This project will give you the means to control a 12-24V DC motor to tune the capacitor. This is required because magloop antennas have an extremely high Q value, resulting in a very narrow usable frequency range. You have to re-tune the antenna almost everytime you change frequencies. 
The benefit of magloops is that they are very small and have extremely good performance for their size. They typically outperform dipoles while needing only a fraction of the space. They're also much less noisy thanks to their high Q. 
I built my magloop out of a piece of high current cable, a vacuum varicap I got off ebay, some PVC pipes and zipties, and it works great from 7 to 28MHz.

## Operation

This controller is very simple: You get two buttons to tune up and down. There's no built in SWR meter so you will have to supply your own, or just tune by ear. Listen for a sharp increase in static noise as you tune across the band. Use your SWR meter on transmit to tune for lowest SWR. Note that you should reduce your transmit power to 5 watts or less in order to not wipe out the wireless RF link for the controller. A magnetic loop produces very strong magnetic fields, so using ferrite rings as RF choke is strongly recommended. Depending on your setup you might not be able to tune the antenna while transmitting. In these cases, tune between transmitting. 

## How it works

This controller uses two low cost NRF24L01 transceivers to transmit tuning commands and endstop switch data. This info is used on the antenna side to stop the motors, and also transmitted to the remote side to light up a status LED. Also, each change in controller state is acknowledged by a LED on the remote, so the operator can see whether or not the antenna received the button presses.

## Lessons Learnt (aka TODO)

Building such a magnetic loop yourself costs around 200 bucks, whereas a commercial solution costs at least 500 USD. However, the DC motor control could be improved by adding a PWM-driven "fine tune" mode. It's currently quite difficult to tune the antenna, especially on the 20m band. Also, the varicap I used has a 24V DC motor. It works with 12V, but kinda struggles near one of the end stops.
Last but not least, I have to turn down the transmit power if I want to tune it. That's not ideal but what can you do.

I'm documenting this little project, imperfect as it is, in the hope that it might be of use to you for your own magloop tuning projects. Perhaps someone can add a SWR-measuring circuit and a true stepper motor control. I'm surprised that no decently-priced commercial magloop exists that has stepper motor tuning...


