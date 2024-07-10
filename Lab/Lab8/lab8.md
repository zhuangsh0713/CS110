# Lab 8

## Exercise 1

> Let the propagation delay of an adder block be 45 ns, the propagation delay of a MUX be 20 ns, and the propagation delay of a shifters block be 5 ns (since we have a constant offset, it is very efficient). The register has a CLK-to-Q delay of 10 ns, a setup time of 10 ns, and a hold time of 5 ns. Calculate the maximum clock rate at which this circuit can operate, assuming that both inputs come from clocked registers that receive their data from an external source.

| Block       | Latency |
| ----------- | ------- |
| adder       | 45ns    |
| multiplexer | 20ns    |
| shifter     | 5ns     |
| CLK-to-Q    | 10ns    |
| setup time  | 10ns    |
| hold time   | 5ns     |

- critical path latency:

    $t_{clk-to-q} + (t_{MUX} + t_{add}) \times 4 + t_{setup} = 10 + (20 + 45) \times 4 + 10 = 280$ ns

- maximum clock frequency: 

    - 3.5714 MHz

    

    

## Exercise 2

![pipelined circuit](/Users/leo/Documents/Computer Architechure/Labs/Lab8/pipelined_circuit.png)

Critical path latency ——110ns

1. $t_{clk-to-q} + t_{MUX} + t_{setup} = 10 + 20 + 10 = 40$ ns
2. $t_{clk-to-q} + t_{add} + t_{add} + t_{setup} = 10 + 45 + 45 + 10 = 110$ ns



Maximum clock frequency: 9.09 MHz

