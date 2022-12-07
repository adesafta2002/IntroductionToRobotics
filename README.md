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

# Homework 4
Use the joystick to move through the 4 digit 7 segment displays digits, press the button to lock in on the current digit
and use the other axis to increment or decrement the number. Keep the button pressed to reset all the digit values and the 
current position to the first digit in the first state.

The system has the following states:

    State 1 (default, but also initiated after a button press in State 2): You can use a joystick axis to cycle through the 4 
    digits; using the other axis does nothing. A blinking decimal point shows the current digit position. When pressing the 
    button, you lock in on the selected digit and enter the second state.
    
    State 2 (initiated after a button press in State 1): In this state, the decimal point stays always on, no longer blinking 
    and you can no longer use the axis to cycle through the 4 digits. Instead, using the other axis, you can increment on 
    decrement the number on the current digit IN HEX. Pressing the button again returns you to the previous state. Also, keep 
    in mind that when changing the number,you must increment it for each joystick movement - it should not work continuosly 
    increment if you keep the joystick in one position.
    
    Reset:  Toggled by long pressing the button only in the first state. When resetting, all the digits go back to 0 and the 
    current position is set to the first (rightmost) digit, in the first state.

<img src="https://user-images.githubusercontent.com/74547133/201972344-b9d4d6d6-c0c5-4499-b01a-002da559a464.jpeg" width="400" height="300">
Demo: https://youtu.be/Y2FF3KUiv8w

# Homework 5
Implement snake using 8x8 matrix, joystick and an LCD display. When program first starts a greeting message is displayed. After
pressing on the joystick.
You can cycle through the menu (UP/DOWN) or press on every item:

    Pressing on Start Game puts you in game mode. 
    
    Pressing on Highscore will show top 5 users. You can go back by pressing on joystick.

    Pressing on Settings opens a new menu where you can change preferences. You can edit a setting by clicking on it and moving
    UP or DOWN and save it by clicking again.
    
    Pressing on About shows you info about the creator of the game and game name (you can see the whole text by moving LEFT/RIGHT)
    
    Pressing on How to play gives you tips to be better at the game (you can see the whole text by moving LEFT/RIGHT)
    
    
<img src="https://user-images.githubusercontent.com/74547133/206240347-7feea5ee-9d5f-4c8f-bd11-931815201b3a.jpeg" width="400" height="300">
Demo: https://youtu.be/Y2FF3KUiv8w
    
