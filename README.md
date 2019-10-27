Arduino: Testing Push Button Quality
------------------------------------

System LED:  
- steady  : indicates ready for push 
- blinking: buffer overflow: to many High, Low oscillations
  
Output:
- Duration (in number of loop cycles) of HIGH, LOW, HIGH, LOW,....
- All numbers are shown in hex 

<pre>
     5V        D4
     ^     o    .
     |    /     |     +-----+
     +---+  ----+-----| 10k |---+
                      +-----+   |
                               ___
                                _
</pre>

<pre>
Push Button Quality Test
Relation between time and cycles:
+-time--+-cycles-+
| 1 sec |  2BAC3 |
|100 ms |   45E0 |
| 10 ms |    6FC |
+-------+--------+
Every press on button produces the following line:
The number (hex) of cycles related to Hi,Lo,Hi,Lo,....
1*5 means 1 1 1 1 1
The last value is maximal (over multiple tests) bouncing time in nSec
Example output:
 59BA A 1*C9 1179ns
 584D 1179ns
2 1 66C4 1179ns
 5916 1179ns
 5A36 1*23 1179ns
 6AA9 1E 1*58 1179ns
 6207 1179ns
</pre>
