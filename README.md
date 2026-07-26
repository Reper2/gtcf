# Grand Tour Cold Front

### Jump to section
[-> Manual](#Manual)  
[-> Legal](#Legal)

## University project remastered
This project was originally written in Processing (Java) for Introduction to Computer Programming [COMP1000]. It has been rewritten line by line by hand with some help from Gemini while I was learning the language in the weekend I started this C++ version right after completing the assignment. The game itself is almost exactly the same in appearance, but has much stricter variable declarations to optimise for performance in this version. For the assignment, I had to enforce sixty frames per seconds and upscale to 144 using a global variable throughout the program. In this version, with variables marked as static, constants, and/or constant expressions, and `int` replaced with `unsigned` wherever possible, the upscaling was not needed as the program is able to run fast enough. Java is also notably much slower, so the transition to a much lower-level language has also massively improved performance in comparison. However, rendering was much crisper in the Processing version. You are more than welcome to contribute to this project if you can improve the rendering without adding any bulky libraries.
### The original assignment scored 35/40, and I received a high distinction with a mark of 88 for the unit.

#### These are the results for the handover rubric:
* **Does it run?:** 2 out of 2.
* **Does it incorporate an aspect from the semester topic?:** 1 out of 2.
* **Does the programs use 2D shapes, variables, arithmetic, if-statements, and event handlers?:** 3 out of 3.
* **Does the program demonstrate mastery of loops, arrays and iteration over arrays, functions (or methods), and self-defined objects?:** 5 out of 5.
<details>
  <summary>Familiarity and Ownership: 8 out of 10.</summary>

  **Context & Background**
The handover quiz was designed to verify that students fully understood and wrote their own code. As someone who values writing and understanding code directly rather than relying on "vibe coding," I really appreciated this assessment format.  
**Quiz Performance**  
On the Friday morning of the quiz, I was running on too much coffee and very little sleep, which led to a few minor line-referencing mistakes. While using my IDE's "Show Usage" feature, I accidentally referenced lines where variables were declared instead of where they were actually initialized or called. While I don't have access to the exact quiz results, the marker later confirmed via email that these line discrepancies were the reason for the small penalty.  
**Grade Adjustment & Outcome**  
The project's grading originally applied conditional penalties because the marker overlooked my program and my incomplete header comments, initially resulting in a score of 16/40. After bringing this up and having the submission reviewed, the grade was corrected twice, ultimately landing on a final score of 35/40 (87.5%).
</details>

#### Your submission and handover meet the minimum requirements. These are the marks for the quality criteria:
* The program demonstrates competent and semantically correct use of control structures: 6 out of 6
<details>
  <summary>
    The program uses arrays to store multiple values of the same type. It uses loops to iterate over the arrays, accessing and updating elements, applying standard array algorithms when necessary, and using functions that operate on arrays for this purpose where possible: 5 out of 6
  </summary>
  I did not receive specific feedback on this, but I do know that there was a big section of my Spaceship code where I just wrote the raw shape calls rather than building arrays to iterate over in loops. There were other places where I could have made helper functions as shape factories to iterate over arrays provided as arguments.
</details>
<details>
  <summary>
    The program is intentionally designed, using user-defined classes and object with best practices like manageable methods size, proper use of parameters and return values, and appropriate variable scope to ensure maintainability and clean code: 5 out of 6
  </summary>
  I could have used at least a dozen more helper functions in my original code, and perhaps even this version. I think the main drawback in that one mark was the length of some of my methods and even classes in general. I thought classes were meant to be that long!
</details>
With a more flexible evaluation scheme across all criteria, I believe my work demonstrated a level of mastery closer to 95%. The primary deduction occurred during the Programming Skills Demonstration test, where I lost 5 marks for "code comprehension." In this task, I accidentally added a duplicate if condition directly below an existing if block inside void keyPressed(). Although the logic executed as intended and successfully met all functional requirements, the inclusion of the redundant condition resulted in receiving zero marks for that specific question.

# Manual
## macOS Installation Note: > Because this build is not signed with an Apple Developer certificate, macOS Gatekeeper may block it on first launch.
To open the game: Right-click GrandTourColdFront → Click "Open" → Click "Open" again.

* Use <kbd>W</kbd><kbd>A</kbd><kbd>S</kbd><kbd>D</kbd>, arrow keys or touch the screen to move the ships through space
- *Note: Once the main ship dies, you cannot move the ships*
* Use <kbd>Z</kbd> to shoot a Fireball in the direction of your mouse position from the nose of the ships
* Use <kbd>X</kbd> to shoot an Iceball in the direction of your mouse position from the nose of the ships
- *Note: Shooting aliens with fireballs directly will award you more points, however the cooldown is shorter for iceballs*
* Use <kbd>C</kbd> to flap Main Penguin's arms
* Use <kbd>V</kbd> to flap Red Penguin's arms
* Use <kbd>B</kbd> to flap Green Penguin's arms

## Legal
© 2026 Ethan Graham. All rights reserved.

This project is distributed under a custom license. It is **Source-Available** for educational and system design reuse, but standalone distribution or low-effort cloning is strictly prohibited unless abandoned. 

* **Source Code:** Permitted for logic and architectural design reuse in original projects. 
* **Media & Screenshots:** Subject to Nintendo's non-commercial capture button policies.
* **Fonts:** Subject to original creator EULAs.

Please read the full terms and conditions in the [LICENSE.txt](./LICENSE.txt) file included in this repository.