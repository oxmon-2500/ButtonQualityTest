Arduino: Testing Push Button Quality
------------------------------------

<pre>
     5V        Dx
     ^     o    ^
     |    /     |     +-----+
     +---+  ----+-----| 10k |---+
                      +-----+   |
                               ___
                                _
</pre>

System LED (D13):  
- steady  : indicates ready for push 
- blinking: buffer overflow: to many High, Low oscillations
  
Output:
- Duration (in number of loop cycles) of HIGH, LOW, HIGH, LOW,....
- All numbers are shown in hex 

<pre>
Every press on button produces the following line: HiLo [ HiLo[*CT] ... ] BT ns
HiLo - the number (hex) of cycles related duration of high/low signal
ct   - repetition;  1*5 means 1 1 1 1 1
BT   - bouncing time in nSec, maximum over multiple tests

Example output:

Push Button Quality Test
Relation between time and cycles:
+-time--+-cycles-+
| 1 sec |  2BAC3 |
|100 ms |   45E0 |
| 10 ms |    6FC |
+-------+--------+
 59BA A 1*C9 1179ns
 584D 1179ns
2 1 66C4 1179ns
 5916 1179ns
 5A36 1*23 1179ns
 6AA9 1E 1*58 1179ns
 6207 1179ns
</pre>
