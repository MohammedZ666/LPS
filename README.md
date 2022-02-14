# Local postioning system with ESP32 chips
## Working principle ##
Any point `(x,y)` in a completely positive number based co-ordinate system can be explained in terms of two circles. Where `(h0,k0)`, `(h1,k1)` is the center and `r0` and `r1` are the distances (and also radii) of the point from the center. 

```
( x - h0 ) + ( y - k0 ) = (r0)**2 ---(i)
( x - h1 ) + ( y - k1 ) = (r1)**2 ---(ii)
```

We can solve these two equation for x and y. Since there are two unknowns and two equations, we can solve for x and y, which will reveal the positition of the point. This is the whole idea behind this local positioning system. 

We can place master (ESP32/8266) with their center or `(h,k)` hard-coded into them to transmit packets to all slaves (ESP32/8266) in an area. For which the slaves will be able to solve for their position in the local-coordinate system with the position and distance data from two such master. Here distance can be measured using this [methodology](https://www.researchgate.net/publication/322877438_Estimate_distance_measurement_using_NodeMCU_ESP8266_based_on_RSSI_technique) via the RSSI strength.

Please flash `master.ino` to master esp32/8266 nodes and `slave.ino` to slave esp32/8266 nodes.

This methodology was not tested extensively for its correctness.
 
