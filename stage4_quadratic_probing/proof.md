# Why Quadratic Probing Requires a Prime Table Size

## Background

In quadratic probing, when a collision occurs at index $h(k)$, we probe at:

$$h(k) + 0^2, \quad h(k) + 1^2, \quad h(k) + 2^2, \quad \ldots \pmod{n}$$

The critical question: **does this sequence visit enough distinct slots to guarantee finding an empty one?**

If the probe sequence cycles back and repeats too early, you might fail to find an empty slot even when one exists — causing an infinite loop.

---

## The Theorem

> **Theorem:** If $n$ is prime, the first $\lfloor n/2 \rfloor + 1$ probes visit **distinct slots**.

This means with a prime-sized table and load factor $< 0.5$, quadratic probing is **guaranteed** to find an empty slot.

---

## Proof by Contradiction

Assume two different steps $i$ and $j$, where $0 \leq i < j \leq \lfloor n/2 \rfloor$, land on the same slot:

$$h(k) + i^2 \equiv h(k) + j^2 \pmod{n}$$

Subtract $h(k)$ from both sides:

$$i^2 \equiv j^2 \pmod{n}$$

Rearrange:

$$j^2 - i^2 \equiv 0 \pmod{n}$$

Factor:

$$(j - i)(j + i) \equiv 0 \pmod{n}$$

This means $n \mid (j-i)(j+i)$.

Since **$n$ is prime**, it must divide at least one of the two factors. We examine both cases.

---

### Case 1: $n \mid (j - i)$

Since $0 \leq i < j \leq \lfloor n/2 \rfloor$:

$$0 < j - i \leq \lfloor n/2 \rfloor < n$$

So $j - i$ is strictly between $0$ and $n$. A number smaller than $n$ cannot be divisible by $n$.

$$\Rightarrow \text{Contradiction.}$$

---

### Case 2: $n \mid (j + i)$

Since $n$ is an odd prime, $\lfloor n/2 \rfloor = \dfrac{n-1}{2}$. Therefore:

$$j + i \leq 2 \cdot \lfloor n/2 \rfloor = 2 \cdot \frac{n-1}{2} = n - 1 < n$$

And since $i \geq 0$ and $j \geq 1$:

$$j + i \geq 1$$

So $j + i$ is strictly between $0$ and $n$, and cannot be divisible by $n$.

$$\Rightarrow \text{Contradiction.}$$

---

## Conclusion

Both cases lead to a contradiction, so **no two probes in the first $\lfloor n/2 \rfloor + 1$ steps can land on the same slot**.

For a prime $n$, this gives us:

$$\lfloor n/2 \rfloor + 1 = \frac{n-1}{2} + 1 = \frac{n+1}{2} \text{ distinct slots guaranteed}$$

So if the load factor is kept below $0.5$ (at most $\lfloor n/2 \rfloor$ slots filled), we are **certain** to find an empty slot.

---

## Why Non-Prime Breaks This

If $n$ is composite, the key step fails: a prime $n$ forces $n \mid (j-i)$ or $n \mid (j+i)$, but a composite $n$ can be "split" across both factors without dividing either individually.

**Example with $n = 9$:**

$$i = 3, \quad j = 6 \implies (j-i)(j+i) = 3 \times 9 = 27$$

Wait — actually let's use the probe offsets directly. With $n = 8$:

| $i$ | $i^2$ | $i^2 \bmod 8$ |
|-----|--------|----------------|
| 0   | 0      | 0              |
| 1   | 1      | 1              |
| 2   | 4      | 4              |
| 3   | 9      | **1** ← repeat |
| 4   | 16     | **0** ← repeat |

With $n = 8$, only **3 distinct offsets** $\{0, 1, 4\}$ are ever reachable — 5 slots are completely **unreachable** regardless of where $h(k)$ lands.

**Example with prime $n = 7$:**

| $i$ | $i^2$ | $i^2 \bmod 7$ |
|-----|--------|----------------|
| 0   | 0      | 0              |
| 1   | 1      | 1              |
| 2   | 4      | 4              |
| 3   | 9      | **2**          |

All 4 values in $\{0, 1, 4, 2\}$ are distinct — exactly $\lfloor 7/2 \rfloor + 1 = 4$ unique slots as the theorem promises.

---

## Summary

| Property | Requirement |
|----------|-------------|
| Table size $n$ | Must be **prime** |
| Load factor | Must stay **below 0.5** |
| Distinct slots guaranteed | $\lfloor n/2 \rfloor + 1 = \dfrac{n+1}{2}$ |
| Why prime? | Forces $n$ to divide a single factor — enabling the contradiction |
