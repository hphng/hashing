# Why Double Hashing Visits All Slots

## The Probe Sequence

Given two hash functions h1 and h2, the probe sequence is:

$$h(k, i) = (h_1(k) + i \cdot h_2(k)) \bmod n \quad \text{for } i = 0, 1, 2, \ldots$$

The offsets from the starting position are:

$$0, \quad h_2(k), \quad 2 \cdot h_2(k), \quad 3 \cdot h_2(k), \quad \ldots \pmod{n}$$

The question: does this sequence visit **all $n$ slots** before repeating?

---

## The Theorem

> **Theorem:** The probe sequence visits all $n$ distinct slots if and only if $\gcd(h_2(k),\ n) = 1$.

---

## Proof

Assume two different steps $i$ and $j$ (where $0 \leq i < j \leq n-1$) land on the same slot:

$$i \cdot h_2(k) \equiv j \cdot h_2(k) \pmod{n}$$

Subtract:

$$(j - i) \cdot h_2(k) \equiv 0 \pmod{n}$$

This means $n \mid (j - i) \cdot h_2(k)$.

**Case: $\gcd(h_2(k),\ n) = 1$**

Since $n$ shares no common factor with $h_2(k)$, it must divide $(j - i)$ entirely:

$$n \mid (j - i)$$

But $0 < j - i < n$, so $n$ cannot divide $(j - i)$. **Contradiction.**

Therefore no two probes collide — all $n$ slots are visited before any repeat.

**Case: $\gcd(h_2(k),\ n) = d > 1$**

Let $h_2(k) = d \cdot a$ and $n = d \cdot b$. Then:

$$(j - i) \cdot d \cdot a \equiv 0 \pmod{d \cdot b}$$
$$(j - i) \cdot a \equiv 0 \pmod{b}$$

This can be satisfied with $j - i = b = n/d < n$. So the sequence **repeats after $n/d$ steps**, visiting only $n/d$ distinct slots — not all $n$.

---

## Why Prime $n$ Guarantees $\gcd(h_2,\ n) = 1$

A prime $n$ has no divisors except $1$ and $n$ itself.

$h_2(k)$ is always in range $[1,\ n-1]$ (ensured by the formula — see below).

For any value $x$ in $[1,\ n-1]$:

$$\gcd(x,\ n) = 1 \quad \text{since } x < n \text{ and } n \text{ is prime}$$

So prime $n$ **automatically guarantees** $\gcd(h_2, n) = 1$ for any valid $h_2$. No extra check needed.

---

## Why $h_2$ Must Never Be Zero

If $h_2(k) = 0$:

$$h(k, i) = (h_1(k) + i \cdot 0) \bmod n = h_1(k) \bmod n \quad \text{for all } i$$

Every probe lands on the same slot — **infinite loop**.

---

## The Complementary Hash Formula

The standard formula for $h_2$ (Knuth's variant):

$$h_2(k) = P - (h_1(k) \bmod P)$$

Where $P$ is a prime and $P < n$.

**Why this is never zero:**

$$h_1(k) \bmod P \in [0,\ P-1]$$
$$P - 0 = P \quad \text{(maximum)}$$
$$P - (P-1) = 1 \quad \text{(minimum)}$$

So $h_2(k) \in [1,\ P] \subseteq [1,\ n-1]$. Never zero, always in valid range.

---

## Load Factor

Because $\gcd(h_2, n) = 1$ guarantees all $n$ slots are reachable (unlike quadratic probing which only covers $\lfloor n/2 \rfloor + 1$), double hashing can safely use a higher load factor:

| Strategy | Max safe load factor | Slots reachable |
|---|---|---|
| Quadratic probing | 0.5 | $\lfloor n/2 \rfloor + 1$ |
| Double hashing | 0.7 | All $n$ |

---

## Summary

| Requirement | Reason |
|---|---|
| Table size $n$ must be prime | Guarantees $\gcd(h_2, n) = 1$ automatically |
| $h_2$ must never be 0 | Prevents infinite loop on first slot |
| $P$ must be prime and $< n$ | Ensures $h_2 \in [1, n-1]$, coprime with $n$ |
| $\gcd(h_2, n) = 1$ | Guarantees all $n$ slots are visited |
