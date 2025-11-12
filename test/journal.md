# What is a Functional Programming Language: Featuring Elixir

![Jet Brains Logo](https://hhargreaves.net/journal/elixir-logo.png)

<br>

###### Author: Hayden Hargreaves

###### Published: 05/21/2025

## Background

Many programmers tend to avoid **functional programming** due to its perceived complexity, myself included.
But is functional programming really that much more complex? My goal is to break my fear of functional 
programming, [monads](https://en.wikipedia.org/wiki/Monad_(functional_programming)), [functors](https://en.wikipedia.org/wiki/Functor), and all those *scary* terms frequently tossed around in the functional
space.

How am I going to do this? Well, I am definitely not going to start with [Haskell](https://www.haskell.org). For those who have 
never seen or heard of Haskell, this decision might be hard to understand. I will not explain myself too 
much. But I will provide a code snippet from the Haskell docs and let you decide if it a worthy language
for someone who has never written a line of functional code in their life.

```haskell
primes = filterPrime [2..] where
  filterPrime (p:xs) =
    p : filterPrime [x | x <- xs, x `mod` p /= 0]
```

Personally, the Haskell language is far too esoteric for my liking. Eventually, I would love to be able 
to, at the very least, **read** Haskell code, but not yet. So this put me a spot to select from a large
selection of languages. I did not want to use a language like **Python** or **JavaScript** which can be 
written to *seem* functional. No, I wanted to write a **real** functional language. A common list of as 
follows:

- [Haskell](https://www.haskell.org)
- [Erlang](https://www.erlang.org)
- [Elixir](https://elixir-lang.org)
- [Scala](https://www.scala-lang.org)
- [Clojure](https://clojure.org)
- [OCaml](https://ocaml.org)
- [Common Lisp](https://lisp-lang.org)

To avoid offending all of the "functional bros" I will not explain my thought process much. However, I 
will mention that I tried to learn **OCaml** a years ago and could not enjoy it. Maybe that was because I 
was not as advanced as I am today, or maybe I did not have the proper mindset. Regardless, that ruled 
out OCaml. I have no experience in any of the other languages, so I did what most would do. I let Reddit
decide! The [Elixir](https://www.reddit.com/r/elixir/) Reddit page was very informative as well as providing me with a link to the 
[2024 Stack Overflow Survey](https://survey.stackoverflow.co/2024/technology#top-paying-technologies) which described Erlang and Elixir as the top two paying languages. 

With that out of the way, lets talk more about what functional programming is.

## What is Functional Programming

At the most basic level, functional programming is defined as a ["programming paradigm where programs are 
constructed by applying and composing functions"](https://en.wikipedia.org/wiki/Functional_programming). In laymen terms, a majority of the code written in these
languages is just functions. Everything can be expressed as some composition of functions. For this reason,
a strong understanding of mathematics can be hugely beneficial to functional programmers.

>
> "Functional programming evolved from lambda calculus"
>

#### Pure Functions

Another key difference between other paradigms is the strict immutability. Functional programming introduces 
the term "[pure function](https://en.wikipedia.org/wiki/Pure_function)" which is any function that can be run (any amount of times) and will always produce
the same output, **and cannot be affected by (or affect) any mutable state.** The intention of writing pure 
functions is to prevent [side effects](https://en.wikipedia.org/wiki/Side_effect_(computer_science)), "any observable effect other than its primary purpose."

To simplify, the two properties of a **pure** function are:

1) The function will return an identical result for identical arguments. With ***no*** variation for any reason, 
including reference arguments.
2) The function has no *side effects*, no mutation of local static variables, non-local variables, etc.)

An example of a pure function in C++ may look as follows:

```c++
void f() {
    static std::atomic<unsigned int> x = 0;
    ++x;
}
```

The function `f()` is pure because it follows the above properties.

```c++
int f_i() {
    static int x = 0;
    ++x;
    return x;
}
```

However, the function `f_i()` is impure because it returns a variation of a static variable.

Countless more examples of impure functions can be found [here](https://en.wikipedia.org/wiki/Pure_function#Impure_functions).

**NOTE:** I wanted to write these examples in Elixir, but due to its functional nature, it would be very hard.

<br>

#### Functions are First Class Citizens

Another key property of functional programming is that functions are known as [first class citizens](https://en.wikipedia.org/wiki/First-class_citizen) which 
means they can be assigned to variables, passed into other functions as arguments ([higher order functions](https://en.wikipedia.org/wiki/Higher-order_function))
and returned from functions. This is not uncommon in modern programming languages so I will not provide an 
in-depth explanation.

<br>

#### Recursion

The death of many modern programming languages, recursion, is one of the many strengths of functional languages.
[Recursion](https://en.wikipedia.org/wiki/Recursion_(computer_science)) occurs when a functions calls itself. This is the most common way to implement [iteration](https://en.wikipedia.org/wiki/Iteration) in 
functional programming. For example a simple loop in any common procedural language:

```c++
std::vector<int> numbers = {1, 2, 4, 8, 16};

for (size_t i = 0; i < numbers.size(); i++) {
    std::cout << numbers[i] << " ";
}
```

Can be written in a similar way using recursion with Elixir:

```Elixir
def print_list([]), do: IO.puts("") # Base case: when the list is empty, this function is called

def print_list([head | tail]) do
    IO.puts(head)      # Print the first element in the list (head)
    print_list(tail)   # Call the function again with the remaining elements
end
```

Programmers coming from other languages might freak when they see so much recursion, and it is not wrong to 
worry. In procedural languages, recursion requires the stack to keep a record of each function call, and when
recursion is used to extreme levels, the stack becomes very large and uses lots of memory. Not very efficient!
However, functional programming languages found a solution, [tail recursion](https://en.wikipedia.org/wiki/Tail_call). Tail recursion allows the compiler
to creation a subroutine which represents the final action (final function call) and place it onto the stack, 
allowing the program to jump right to the end without having to store the entire recursive loop in stack memory.
This reduces the memory stack space from linear or O(N) to constant or O(1). A huge performance increases in 
recursive programs.

<br>

#### Strict vs. Non-Strict Evaluation

Functional languages can be categorized by *strict (eager)* or *non-strict (lazy)* evaluation. The different 
evaluation styles refer to how arguments are processed inside an expression under evaluation. This concept 
is quite complex and this overview does not warrant the necessity for such details. However, a simple example
can explain (on a higher level) the difference. **"Under strict evaluation, the evaluation of any term containing
a failing subterm fails.** An example makes this a bit easier to understand. We will use the same example to described
both evaluation types.

```
print length([2+1, 3*2, 1/0, 5-4])
```

In a language categorized as *strict*, the expression above will fail, due to the term `1/0` failing. However, in a language
with *non-strict* evaluation, the length expression will return 4 because it does attempt to evaluate the subterms, which
result in failure.

To summarize, *lazy* evaluation does not attempt to evaluate function arguments unless their values are required for the 
function call itself.

***NOTE: Elixir is a strictly evaluated language.***

<br>

#### Referential Transparency

Another very detailed differentiation from imperative programming languages. [Referential transparency](https://en.wikipedia.org/wiki/Referential_transparency) stems from linguistic 
roots and language by extension. Applications in this context (computer science) states that "a language is *referentially
transparent* when an expression built from another, replaces the expression with a subexpression which represents the same value
and does not change the value of the expression." That sounds very complicated, and it is, but to be simple, expressions cannot 
be modified, only replaced. 

Functional programming languages do not have **assignment statements**, because a value can never be changed once it is defined.
This is a result of the language allowing variables to be swapped with their value at any time. Therefore, functional languages
are referentially transparent.

Another example will make this easier to understand. Consider the following assignment statement from [C](https://en.wikipedia.org/wiki/C_(programming_language)). This assignment changes
the value assigned to the variable `x`. If the initial value of `x` was to be `1`, the result would be `10`. But when called again, the 
result would be `100`. Since replacing `x` with `10` or `100` gives the program different meaning, it is not *referentially transparent.*

```c
x = x * 10
```

However, consider the following elixir function, `f(x)`. This implementation is *transparent*, as it does not modify the value of x,
which results in the absence of **side effects**. Instead, it returns a value which can be used to replace an existing value. This
is the standard in functional languages which results in their referential nature.

```elixir
def f(x) do
    x + 1
end
```

<br>

#### Data Structures

The last key difference between imperative and function languages is their representation of data structures. Functional languages
admit a [purely functional](https://en.wikipedia.org/wiki/Purely_functional_data_structure) data structure, where the biggest difference is immutability. Purely functional data structures are 
strongly immutable, which allows for many advantages, such as [persistence](https://en.wikipedia.org/wiki/Persistent_data_structure), quick copy and [thread safety](https://en.wikipedia.org/wiki/Thread_safety).
Of the advantages listed, I will only go in detail about one: **persistence.** Purely functional data structures are persistent which 
means that when modifications occur, the previous state will be kept unmodified. A common comparison is to the non-persistent array 
which admits a **destructive update** which cannot be undone, since no previous versions are kept.

***NOTE: Elixir is not a purely functional language and as a result, does not implement a purely functional data structure.***


## Why Functional?

That was quite a long explanation, but it covered almost everything you would need to know to *begin* learning a functional language.
However, I did not explain why **I** decided to learn. Functional languages are rarely faster (or even as fast) as imperative languages
like C, so the choice was not made for performance. The most obvious choice: ***I want to.*** So many software developers spend too much 
time worrying about what is "the best" or "the fastest." Maybe they should just be learning what they **want.** 

Like I stated in the background, I, like many others, run at the sight of functional languages, but its about time I break that fear!
The next sections of this article will highlight my experience learning the **Elixir Programming Language,** so if you were only here for 
the functional definition, this is a good stopping point.

## Why Elixir?

I have talked a lot about Elixir language but what exactly is it? Obviously its a functional programming language, but that's very vague.
"[Elixir](https://elixir-lang.org) is a dynamic, functional language for building scalable and maintainable applications." Elixir runs on the [Erlang](https://www.erlang.org) VM which is 
known for creating fault tolerant, low-latency, distributed systems. The language can be installed and testing in the interactive elixir shell 
`iex`, similar to pythons interactive shell. The interactive shell was a great tool for my own learning while reading through the documentation.

Elixir was first released in **2012** which makes it a newer language, but not as new as some (Odin, or Zig). With this comes a large user base
as well as a large developer ecosystem. But, what makes elixir stand out in that aspect is its compatibility with Erlang. Erlang, which appeared
in **1986** has a huge ecosystem of libraries and tools which work seamlessly in Elixir. Elixir comes packaged with a build tool, `mix` which allows
for compilation and interpretation of elixir code. Elixir *also* has a dedicated package manager, `hex`, which is comparable to `npm` in terms of 
use. However, Elixir's ecosystem does not compare to the JavaScript ecosystem (what language does?).

One of the most popular uses of Elixir is with the [Phoenix Web Framework](https://www.phoenixframework.org). Phoenix, a full stack web framework that boasts its countless features 
which include **LiveView**, a tool for building real-time web applications. Another popular library, **[Ecto](https://hexdocs.pm/ecto/Ecto.html)** is a SQL ORM that is built into the Phoenix
framework and allows for seamless database connections from your web application back end. (The Phoenix framework will come up more later.)


But who cares? Well, many large companies you have heard of use Elixir in their software. Some of which include WhatsApp, Discord, Heroku, Pepsico
and [more](https://elixir-lang.org/cases.html).

## The Beginning

Personally, I am of the believe the best (and maybe even only) way to learn a programming language is to build something. No matter how many 
videos you watch, tutorials you read, or examples you look at, you will never fully understand the nuances of a language until you have tried 
it out yourself. Of course, this does assume you have a basic understanding of programming and software design. So, to adhere to my own advice,
the first thing I did was create a small project. The ever-dreaded, **To-do List.** 

I choose this project because it is simple, I have written a million of them, it allows me to learn terminal I/O, file I/O, basic data handling,
and some more complex data types such as maps and lists. Typically, my first go-to application when learning a new language is a simple web server.
But, since I wanted to experiment with **Phoenix** I decided to wait until after getting my hands on the language before trying such a detailed 
framework.

The application can be found on my GitHub [here](https://github.com/haydenhargreaves/ElixirTodo). I would like to note, I did not spend much effort on the repo or making the UI very beautiful.
I hope you can understand that quality was not the goal here. 

After writing this simple CLI tool I felt far more confident in my Elixir ability and the ability to read the docs and find what I was looking for 
without just prompting an LLM to solve my problems.

But that reminds me, I actually lied to you. The first thing I did was *not* write the to-do list. No, the very first thing I did was read through 
a good chunk of the [Elixir Getting Started Guide](https://hexdocs.pm/elixir/introduction.html). In the past few years, I have tried my hand at over a dozen programming languages, many of 
which I gave up on very fast. The most common complaint I have with modern programming languages is their lack of comprehensive documentation. However,
the Elixir documentation *blew my socks off!* The documentation is **amazing.** Not only is it very easy to read, it **makes sense.** I can remember trying
to learn a few languages and trying to read their docs was a nightmare, *cough cough, Zig.* After just a few hours, I had a pretty solid understanding of 
the language at a semantic level and was able to read through more complicated examples with ease. 

The introduction linked above is a complete walk through of *almost* everything you would need to write industry grade software in the Elixir language.
Even after only reading through the first 10 or so sections, I was beyond ready to begin writing code.

The [modules](https://hexdocs.pm/elixir/Kernel.html) segment of the docs is just as powerful, especially once you have jumped into the deep end of writing your own code. Documentation complete 
with syntax, functions, types, a summary and even examples can be found for each and every module in the standard library. It really is the best resource
I have found (so far) for learning the language. Which is not something I can say for other languages.

## An Upgrade

So now I have written something small, and read through the documentation. What's next? Well, a large scale application, duh! For this project, I will build
a full stack web application using the Phoenix Framework. This app will allow users to share, copy, create and search for recipes. I have recently developed 
a love for cooking and having to store all my recipes in my notes app is cumbersome. Plus, my parents are **amazing** cooks and I would to be able to "borrow" 
their recipes and save them for myself, without having to copy them manually.

## Elixir Review

The learning process has been put on pause to focus on work, but when I make it back to 
Elixir, I will finish the conclusion!

