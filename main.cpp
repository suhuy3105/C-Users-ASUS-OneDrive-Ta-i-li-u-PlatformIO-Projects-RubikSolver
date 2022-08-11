#include <Arduino.h>
#include <Servo.h>
//#include <Solver.h>
 
// servo objects
Servo rotate_servo;
Servo push_servo;
 
int move_speed = 8 ; 
int buffer_time = 115; // time between moves
int rotate_pos = 90;
int push_pos = 180;
int hold_progress = 3;
int offset_degrees = 10;
bool slow_push = false;
String kociemba_sol = "";
 
//////// cube move variables:
bool sim_only = false;
 
// test function:
bool test_ongoing = true;
 
////////////////////// Serial Communication (receive arrays from .py script)
 
void accept_string()
{    
        char ready_signal = 'ready';
        char received_signal = 'received';
 
        for (int piece_num = 0; piece_num <5; piece_num++)
        {       
                // send ready signal
                Serial.println(ready_signal);
                delay(100);
        }
        // receive string
        while(kociemba_sol == "")
        {
                char character;
                while(Serial.available())
                {
                    character = Serial.read();
                         kociemba_sol.concat(character);
                }
        }
        delay(10);
        Serial.print("String Aceptado: ");
        Serial.print(kociemba_sol);
 
        // send color confirmed signal
        Serial.println("arduino dice:");
        Serial.println(received_signal);
        Serial.println(kociemba_sol);
        delay(10);
}
 

///////////////////// Physical Movement Functions ///////////////////////////
 
////////// Servo movement function: ///////////
void move_servo(int start, int finish, int servo_pin)
{
        int pos;
        if (start - finish < 0)
        {
                for(pos = start; pos <= finish; pos += 1)
                {
                        if (servo_pin == 6)
                        {
                                push_servo.write(pos);
                                delay(move_speed);
                        }
                        else if (servo_pin == 9)
                        {
                                rotate_servo.write(pos);
                                delay(move_speed);
                        }
                }
        }
        else
        {
                for(pos = start; pos >= finish; pos -= 1)
                {
                        if (servo_pin == 6)
                        {
                                push_servo.write(pos);
                                delay(move_speed);
                        }
                        else if (servo_pin == 9)
                        {
                                rotate_servo.write(pos);
                                delay(move_speed);
                        }
                }
        }
        // use a swich case next time
        if (servo_pin == 9)
        {
                rotate_pos = pos;
        }
        if (servo_pin == 6)
        {
                push_pos = pos;
        }
        delay(buffer_time);
}

void release_cube()
{
        move_servo(push_pos, 180, 6);
        hold_progress = 3;
}
///////// Cube movement functions: ////////////

void push_cube(int num_of_pushes = 1)
{
        if (num_of_pushes == 1)
                {
                        if (slow_push == false)
                        {
                                move_servo(push_pos, 98, 6);
                                delay(buffer_time);
                                release_cube();
                                delay(buffer_time);
                        }
                        else // on rotate one
                        {
                                move_servo(push_pos, 98, 6);
                                delay(buffer_time+200);
                                release_cube();
                                delay(buffer_time);
                        }
                }
        else
        {
                while (num_of_pushes != 0)
                {
                        if (slow_push == false)
                        {
                                move_servo(push_pos, 98, 6);
                                delay(buffer_time+50);
                                move_servo(push_pos, 160,6);
                                delay(buffer_time);
                                num_of_pushes--;
                        }
                        else // on rotate one
                        {
                                move_servo(push_pos, 98, 6);
                                delay(buffer_time+200);
                                move_servo(push_pos, 160, 6);
                                delay(buffer_time);
                                num_of_pushes--;
                        }      
                }
        release_cube();
        }
}
void hold_cube()
{
        move_servo(push_pos, 140, 6);
        hold_progress = 1;
}

void rotate_one()
{
        slow_push = true;
        int rotate_finish = 0;
        if (hold_progress == 1) // hold progress 1 = hold
        {
                // from rotate_two
                if (rotate_pos < 140)
                {
                        // initial turn
                        move_servo(rotate_pos, rotate_finish-9, 9);
                        move_servo(rotate_pos, rotate_finish+10, 9);
                        // release and turn some more
                        release_cube();
                        move_servo(rotate_pos, 101, 9);
                        hold_cube();
                        move_servo(rotate_pos, 64, 9);
                        move_servo(rotate_pos, 92, 9); // prevent pulling
                        release_cube();
                        move_servo(rotate_pos, rotate_finish, 9);
                }
 
                // from rotate_three
                else if (rotate_pos > 140)
                {
                        // initial turn
                        move_servo(rotate_pos, rotate_finish-3, 9);
                        move_servo(rotate_pos, rotate_finish+3, 9);
                        // release and turn some more
                        release_cube();
                        move_servo(rotate_pos, 115, 9);
                        hold_cube();
                        move_servo(rotate_pos, 64, 9);
                        move_servo(rotate_pos, 93, 9); // prevent pulling
                        release_cube();
                        move_servo(rotate_pos, rotate_finish, 9);
                }
                hold_progress = 2;
        }
        else if (hold_progress == 2) // hold progress 2 = release, but offset still there
        {
                hold_progress = 3;
                move_servo(rotate_pos, rotate_finish, 9);
        }
        else if (hold_progress == 3) // hold progress 3 = release, offsets reconciled
        {
                // do nothing
                move_servo(rotate_pos, rotate_finish, 9);
        }
}
void rotate_two()
{
        slow_push = false;
        int rotate_finish = 90;
        if (hold_progress == 1) // hold progress 1 = hold
        {
                // rotate from rotate_one
                if (rotate_pos < 50)
                {
                        // initial turn
                        move_servo(rotate_pos, rotate_finish+10, 9);
                        move_servo(rotate_pos, rotate_finish-5, 9);
 
                        // release and turn some more
                       
                        release_cube();
                        move_servo(rotate_pos, 0, 9);
                        hold_cube();
                        move_servo(rotate_pos, 8, 9);
                        move_servo(rotate_pos, 5, 9); // prevent pulling
                        release_cube();
                       
                        move_servo(rotate_pos, rotate_finish, 9);
                }
                // rotate from rotate_three
                else if (rotate_pos > 150)
                {
                        move_servo(rotate_pos, rotate_finish-12, 9);
                        move_servo(rotate_pos, rotate_finish+4, 9);
 
 
                        // release and turn some more
                        release_cube();
                        move_servo(rotate_pos, 180, 9);
                        hold_cube();
                        move_servo(rotate_pos, 170, 9);
                        move_servo(rotate_pos, 178, 9); // prevent pulling
                        release_cube();
                        move_servo(rotate_pos, rotate_finish, 9);
                }
                hold_progress = 2;
        }
        else if (hold_progress == 2) // hold progress 2 = release, but offset still there
        {
                hold_progress = 3;
                move_servo(rotate_pos, rotate_finish, 9);
        }
        else if (hold_progress == 3) // hold progress 3 = release, offsets reconciled
        {
                // do nothing
                move_servo(rotate_pos, rotate_finish, 9);
        }
}
void rotate_three()
{
        slow_push = false;
        int rotate_finish = 180;
        if (hold_progress == 1) // hold progress 1 = hold
        {
                // from rotate_two
                if (rotate_pos > 40)
                {
                        move_servo(rotate_pos, rotate_finish+4, 9);
                        move_servo(rotate_pos, rotate_finish-5, 9); // prevent pulling
 
                        // fix: cube not fully turned
                        release_cube();
                        move_servo(rotate_pos, 80, 9);
                        hold_cube();
                        move_servo(rotate_pos, 104, 9);
                        move_servo(rotate_pos, 90, 9); // prevent pulling
                        release_cube();
                        move_servo(rotate_pos, rotate_finish, 9);
                }
 
                // from rotate_one
                if (rotate_pos < 40)
                {
                        move_servo(rotate_pos, rotate_finish+5, 9);
                        move_servo(rotate_pos, rotate_finish-7, 9); // prevent pulling
 
                        // fix: cube not fully turned
                        release_cube();
                        move_servo(rotate_pos, 70, 9);
                        hold_cube();
                        move_servo(rotate_pos, 85, 9);
                        move_servo(rotate_pos,105, 9); // prevent pulling
                        release_cube();
                        move_servo(rotate_pos, rotate_finish, 9);
                }
 
                hold_progress = 2;
        }
        else if (hold_progress == 2) // hold progress 2 = release, but offset still there
        {
                hold_progress = 3;
                move_servo(rotate_pos, rotate_finish, 9);
        }
        else if (hold_progress == 3) // hold progress 3 = release, offsets reconciled
        {
                // do nothing
                move_servo(rotate_pos, rotate_finish, 9);
        }
}
 
///////////////////// Cube Move Notation ///////////////////////////
// They print, simulate and call the physical functions
 
void right_inverted()
{
        Serial.println("R', ");
 
        if (sim_only == false)
        {
                rotate_three();
                push_cube();
                hold_cube();
                rotate_two();
                release_cube();
                rotate_one();
                push_cube();
                rotate_two();
                push_cube(3);
        }
 
}
void right()
{
        Serial.println("R, ");
 
        if (sim_only == false)
        {
                rotate_three();
                push_cube();
                rotate_two();
                hold_cube();
                rotate_three();
                release_cube();
                rotate_one();
                push_cube();
                rotate_two();
                push_cube();
        }
 }
void left_inverted()
{
        Serial.println("L', ");
 
        if (sim_only == false)
        {
                rotate_one();
                push_cube();
                rotate_two();
                hold_cube();
                rotate_one();
                release_cube();
                rotate_three();
                push_cube();
                rotate_two();
                push_cube();
        }
 
}
void left()
{
        Serial.println("L, ");
 
        if(sim_only == false)
        {
                rotate_one();
                push_cube();
                hold_cube();
                rotate_two();
                release_cube();
                rotate_three();
                push_cube();
                rotate_two();
                push_cube(3);
        }
 
}
void down_inverted()
{
        Serial.println("D', ");
 
        if (sim_only == false)
        {
                hold_cube();
                rotate_one();
                release_cube();
                rotate_two();
                push_cube();
                rotate_one();
                push_cube();
                rotate_two();
                push_cube(3);
        }
 
}
void down()
{
        Serial.println("D, ");
 
        if (sim_only == false)
        {
                hold_cube();
                rotate_three();
                release_cube();
                rotate_two();
                push_cube();
                rotate_three();
                push_cube();
                rotate_two();
                push_cube(3);
        }
 
}
void up_inverted()
{
        Serial.println("U', ");
 
        if (sim_only == false)
        {
                push_cube(2);
                hold_cube();
                rotate_one();
                release_cube();
                rotate_two();
                push_cube();
                rotate_one();
                push_cube();
                rotate_two();
                push_cube();
        }
 
}
void up()
{
        Serial.println("U, ");
 
        if (sim_only == false)
        {
                push_cube(2);
                hold_cube();
                rotate_three();
                release_cube();
                rotate_two();
                push_cube();
                rotate_three();
                push_cube();
                rotate_two();
                push_cube();
        }
 
}
void front_inverted()
{
        Serial.println("F', ");
 
        if (sim_only == false)
        {
        push_cube(3);
        hold_cube();
        rotate_one();
        release_cube();
        rotate_two();
        push_cube();
        rotate_one();
        push_cube();
        rotate_two();
        }
 
}
void front()
{
        Serial.println("F, ");
 
        if (sim_only == false)
        {
                push_cube(3);
                hold_cube();
                rotate_three();
                release_cube();
                rotate_two();
                push_cube();
                rotate_three();
                push_cube();
                rotate_two();
        }
}
void back_inverted()
{
        Serial.println("B', ");
        if (sim_only == false)
        {
                push_cube();
                hold_cube();
                rotate_one(); // ccw
                release_cube();
                rotate_two();
                push_cube(3);
                rotate_three(); //cw
                push_cube();
                rotate_two();
        }
 
}
void back()
{
        Serial.println("B, ");
 
        if (sim_only == false)
        {
                push_cube();
                hold_cube();
                rotate_three();
                release_cube();
                rotate_two();
                push_cube(3);
                rotate_one();
                push_cube();
                rotate_two();
        }
 
}    

// insert top layer edges

// miscellaneous algorithms
void warm_up() // do it six times to get back to the original position
{
        Serial.println();
        Serial.print("Warmup: ");
        Serial.print("R', D', R, D");
        //r'
        rotate_one();
        push_cube();
        hold_cube();//
        rotate_two();
        release_cube();
        rotate_three();
        push_cube();
        rotate_two();
        push_cube(3);
 
        //d'
 
        hold_cube();//
        rotate_three();
        release_cube();
 
        //r start here
 
        rotate_two();
        push_cube();
        rotate_three();
        hold_cube();
        rotate_two();
        release_cube();
 
        // d
 
        rotate_three();
        push_cube();
        hold_cube();
        rotate_two();
        release_cube();
        push_cube();
        rotate_one();
        push_cube();
        rotate_two();
        push_cube(3);
}
void superflip() // all edges are opposite (checkered pattern)
{
        Serial.println();
        Serial.println("Superflip: ");
        up();
        up();
        down();
        down();
 
        left();
        left();
        right();
        right();
 
 
        front();
        front();
        back();
        back();
}
 
                                                                                                                                                                                // test it
void scramble() // random 25 moves
{
        Serial.println();
        Serial.println("Scramble: ");
        int move;
        for(int j = 0; j < 25; j++)
        {
                move = random(1, 12);
                //Serial.println(move);
                switch(move)
                {
                        case 1:
                                right();
                                break;
                        case 2:
                                right_inverted();
                                break;
                        case 3:
                                left();
                                break;
                        case 4:
                                left_inverted();
                                break;
                        case 5:
                                up();
                                break;
                        case 6:
                                up_inverted();
                                break;
                        case 7:
                                down();
                                break;
                        case 8:
                                down_inverted();
                                break;
                        case 9:
                                front();
                                break;
                        case 10:
                                front_inverted();
                                break;
                        case 11:
                                back();
                                break;
                        case 12:
                                back_inverted();
                                break;
                }
        }
}
 
// test all possible rotation combinations (for mechanical testing)
void rotate_one_to_two()
{
        Serial.println("rotate_one to rotate_two");
        rotate_one();
        hold_cube();
        rotate_two();
        release_cube();
        push_cube();
}
 
void rotate_two_to_one()
{
        Serial.println("rotate_two to rotate_one");
        rotate_two();
        hold_cube();
        rotate_one();
        release_cube();
        push_cube();
}
 
void rotate_two_to_three()
{
        Serial.println("rotate_two to rotate_three");
        rotate_two();
        hold_cube();
        rotate_three();
        release_cube();
        push_cube();
}
 
void rotate_three_to_two()
{
        Serial.println("rotate_three to rotate_two");
        rotate_three();
        hold_cube();
        rotate_two();
        release_cube();
        push_cube();
}
 
// double turns:
void rotate_three_to_one()
{
        Serial.println("rotate_three to rotate_two");
        rotate_three();
        hold_cube();
        rotate_one();
        release_cube();
        push_cube();
}
 
void rotate_one_to_three()
{
        Serial.println("rotate_one to rotate_two");
        rotate_one();
        hold_cube();
        rotate_three();
        release_cube();
        push_cube();
}
void rotation_test()
{
        Serial.println("Rotation Test:");
 
        rotate_one_to_two();
        rotate_two_to_one();
        rotate_two_to_three();
        rotate_three_to_two();
        rotate_three_to_one();
        rotate_one_to_three();
 
        rotate_one();
        push_cube();
        rotate_two();
        push_cube();
        rotate_three();
        push_cube();
       
}


 

void run_kociemba(){


// Length (with one extra character for the null terminator)
int str_len = kociemba_sol.length() + 1; 


Serial.println("arduino dice: Caracteres:");
Serial.println((str_len-1));

for(int i = 0; i <= (str_len-1); i++){     //recorre

//Serial.print(i);


if ((kociemba_sol.charAt(i)) =='R'){


if ((kociemba_sol.charAt(i+1)) == '2') {
      right();
      right();}


if ((kociemba_sol.charAt(i+1)) == '\'') {
      right_inverted();}

if (((kociemba_sol.charAt(i+1))!= '2') and((kociemba_sol.charAt(i+1)) != '\'')) {
      right();}

}

else { //NO HACER NADA
      
}

if ((kociemba_sol.charAt(i)) =='L'){


if ((kociemba_sol.charAt(i+1)) == '2') {
      left();
      left();}


if ((kociemba_sol.charAt(i+1)) == '\'') {
      left_inverted();}

if (((kociemba_sol.charAt(i+1))!= '2') and((kociemba_sol.charAt(i+1)) != '\'')) {
      left();}

}

else { //NO HACER NADA
      
}

if ((kociemba_sol.charAt(i)) =='U'){


if ((kociemba_sol.charAt(i+1)) == '2') {
      up();
      up();}


if ((kociemba_sol.charAt(i+1)) == '\'') {
      up_inverted();}

if (((kociemba_sol.charAt(i+1))!= '2') and((kociemba_sol.charAt(i+1)) != '\'')) {
      up();}

}

else { //NO HACER NADA
      
}



if ((kociemba_sol.charAt(i)) =='D'){


if ((kociemba_sol.charAt(i+1)) == '2') {
      down();
      down();}


if ((kociemba_sol.charAt(i+1)) == '\'') {
      down_inverted();}

if (((kociemba_sol.charAt(i+1))!= '2') and((kociemba_sol.charAt(i+1)) != '\'')) {
      down();}

}

else { //NO HACER NADA
      
}





if ((kociemba_sol.charAt(i)) =='F'){


if ((kociemba_sol.charAt(i+1)) == '2') {
      front();
      front();}


if ((kociemba_sol.charAt(i+1)) == '\'') {
      front_inverted();}

if (((kociemba_sol.charAt(i+1))!= '2') and((kociemba_sol.charAt(i+1)) != '\'')) {
      front();}

}

else { //NO HACER NADA
      
}




if ((kociemba_sol.charAt(i)) =='B'){


if ((kociemba_sol.charAt(i+1)) == '2') {
      back();
      back();}


if ((kociemba_sol.charAt(i+1)) == '\'') {
      back_inverted();}

if (((kociemba_sol.charAt(i+1))!= '2') and((kociemba_sol.charAt(i+1)) != '\'')) {
      back();}

}

else { //NO HACER NADA
      
}

}
   
                
        //warm_up();
        //scramble();
}
 
void show_off_cube()
{
        rotate_one();
        rotate_three();
        push_cube(2);
        rotate_one();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// PROGRAM START ///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
 
void setup()
{
        rotate_servo.attach(9);  // attaches the servo on pin 9 to the servo object
        push_servo.attach(6);  // attaches the servo on pin 6 to the servo object
        push_servo.write(push_pos);
        rotate_servo.write(rotate_pos);
        delay(1000);
        Serial.begin(9600);
        while (! Serial); // Wait untilSerial is ready
       
}
 
/////////////// Se recibe por puerto serial la solucion  //////////////////

void loop()
{      
//recibir la solucion de kociemba por puerto serial


accept_string();


 //calentando 5 segundos

Serial.println("Calentando: volvera a la posicion inicial");


push_cube();
push_cube();
push_cube();
push_cube();

delay(2000);


Serial.println("Arduino dice:Inicia a correr la solucion:");


 run_kociemba(); //corre el string recibido
        
Serial.println("Arduino dice: Finaliza tiempo solucion");

//mostrar el cubo resuelto

show_off_cube();


Serial.println("Arduino dice:Finalizado enviar nueva solucion de Kociemba");
  
        
        while(true){}

  
 
}