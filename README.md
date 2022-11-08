# Introduction to Robotics (2022 - 2023)
  Introduction to Robotics laboratory homeworks, taken in the 3rd year at the Faculty of Mathematics and Computer Science, University of Bucharest. Each homework includes requirements, implementation details, code and image files.

# Homework 1
  Task Requirements: Use a separate potentiometer in controlling each of the color of the RGB led (Red, Green and Blue). The control must be done with digital electronics.
  
<img src="https://user-images.githubusercontent.com/74547133/197804710-96688d40-147e-4db0-ab61-77dccf75a077.jpeg" width="400" height="300">
Demo: https://www.youtube.com/watch?v=u7XZ45WP8H8

# Homework 2

Building the traffic lights for a crosswalk. You will use 2 LEDs to represent the traffic lights for people (red and green)
and 3 LEDs to represent the traffic lights for cars (red, yellow and green). 

The system has the following states:

    State 1 (default, reinstated after state 4 ends): green light for cars,red light for people, no sounds. 
    Duration: indefinite, changed by pressing the button.

    State 2 (initiated by counting down 8 seconds after a button press): the light should be yellow for cars, red for people and
    no sounds. 
    Duration: 3 seconds.

    State 3 (initiated after state 2 ends): red for cars, green for people and a beeping sound from the buzzer at a constant 
    interval. 
    Duration: 8 seconds.

    State 4 (initiated after state 3 ends): red for cars, blinking green for people and a beeping sound from the buzzer, at a 
    constant interval, faster than the beeping in state 3. 
    Duration: 4 seconds.
    
<img src="https://user-images.githubusercontent.com/74547133/199234802-aa1ede86-6fc7-471f-af69-f7f68e91383f.jpeg" width="400" height="300">
Demo: https://youtube.com/shorts/Z2RcCLbM2EU

# Homework 3
Use the joystick to control the position of the segment and ”draw” on the display. The movement between segments
should be natural (meaning they should jump from the current position only to neighbors, but without passing through ”walls”).

The system has the following states:

    State 1 (default, but also initiated after a button press in State 2): Current position blinking. Can use the joystick
    to move from one position to neighbors. Short pressing the button toggles state 2. Long pressing the button in state 1
    resets the entire display by turning all the segments OFF and moving the current position to the decimal point.
    
    State 2 (initiated after a button press in State 1): The current segment stops blinking, adopting the state of the segment
    before selection (ON or OFF). Toggling the X (or Y, you chose) axis should change the segment state from ON to OFF or from
    OFF to ON. Clicking the joystick should save the segment state and exit back to state 1.

<img src="https://user-images.githubusercontent.com/74547133/200623935-c3172aa8-6c11-4ee9-9c1e-598a3790348c.jpeg" width="400" height="300">
Demo: https://www.youtube.com/watch?v=Px-4uAZYgc8


