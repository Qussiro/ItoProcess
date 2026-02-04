\# Simulácia Itôových procesov



Tento program v jazyku C++ s využitím frameworku Qt simuluje a vizualizuje trajektórie vybraných Itôových procesov.



\## Implementované úlohy



\### 1. Wienerov proces ($W\_t$)

Základný proces pre brownov pohyb, kde prírastky $\\Delta W = W\_{t+\\Delta t} - W\_t$ sú rozdelené podľa normálneho rozdelenia $N(0, \\Delta t)$.



\### 2. Brownov pohyb na rotujúcej priamke (Úloha 18.3.2)

Uvažujme priamku rotujúcu konštantnou uhlovou rýchlosťou $\\omega$ v rovine. X-ová súradnica peľového zrnka vykonávajúceho brownov pohyb na tejto priamke je daná vzťahom:

$$X\_t = W\_t \\cos(\\omega t)$$



\### 3. Tieň na vyvíjajúcom sa grafe (Úloha 18.3.8)

Uvažujme brownov pohyb $X\_t = W\_t$ na x-ovej osi a vyvíjajúci sa graf funkcie $f(x, t) = \\frac{1 + x^2}{1 + t}$. Súradnica $y$ tieňa zrnka na tomto grafe je Itôov proces:

$$Y\_t = \\frac{1 + W\_t^2}{1 + t}$$



\## Simulácia a Vizualizácia

Trajektórie sú generované diskretizáciou časového intervalu $\[0, T]$ na $N$ krokov.

1\. Generujeme prírastky Wienerovho procesu $\\Delta W\_i \\sim N(0, \\Delta t)$.

2\. Kumulatívnym súčtom získame diskretizovanú trajektóriu $W\_t$.

3\. Aplikujeme transformáciu pre konkrétny Itôov proces.

4\. Program vykreslí trajektórií do spoločného grafu pre vizuálnu analýzu správania procesu.

