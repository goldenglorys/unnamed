# Janky Lexer: The Second Step

Welcome to the next entry in my Janky Lexer project diary. If you're new here, let me just say that this journey is filled with confusion, Google searches, and a lot of facepalms. But hey, that's what learning is all about, right?

## Experience, Pains, and Difficulties

### The Struggle Continues

After the initial struggle of getting the basic lexer up and running, I realized that my code was a mess. Everything was crammed into one file, and it was becoming increasingly difficult to manage. So, I decided to take a step back and refactor my code.

### Separating Functions into Different Files

The first thing I did was separate the functions into different files. I created a `lexerf.h` header file to declare the functions and structs, and a `lexerf.c` file to implement them. This made the code much cleaner and easier to navigate.

### Running the Code

Once I had everything separated, I ran the code to make sure it still worked. And guess what? It didn't. I spent hours debugging and realized that I had missed a few includes and declarations. But eventually, I got it working again.

### Changing the Logic

Next, I decided to change the logic of the lexer. Instead of generating tokens on the fly, I decided to store them in an array. This way, I could easily access and manipulate the tokens later on.

### Adding More Functionality

Finally, I added more functionality to the lexer. I implemented a function to print the tokens, and I also added support for more keywords and separators. This made the lexer much more robust and useful.

## What I Learned

1. **Organization is Key**: Separating functions into different files makes the code much easier to manage and understand.
2. **Debugging is a Pain**: But it's a necessary evil. The more you debug, the better you get at it.
3. **Refactoring is Important**: Sometimes, you need to take a step back and refactor your code to make it better. It's a lot of work, but it's worth it in the end.

## What's Next?

Next up, I'll be working on the parser..while improving lexer and everything in between on the fly. I'll be doing more tests (manual) and edge cases to make sure it can handle anything I throw at it.

### Commit Link

You can check out the code for this step [here](https://github.com/goldenglorys/unnamed/commit/52e13db010834d7f023465dd42756cd490c3b77a).

So there you have it. The next step of my janky lexer journey. It's been a wild ride so far, but I'm learning a lot and having fun (most of the time).

---

Happy coding, fellow strugglers! 🚀💻😵‍💫

---