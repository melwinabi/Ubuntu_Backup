// operations in js

// arithemeatic

// variables + operations = expression (algorithm)
// arithematic operatord (+.-,*,/,%,** : numb 1 power of num 2)
//inc (++) , dec (--)
let num1 = 56;
let num2 = 77;
let res = (num1*num2);
console.log(res)
 
//comparitive
// operators (<,>,<=,>=,==,!==)
console.log(8>0)   // output will be in boolean (true or false)
console.log(num1==num2)

// strin comparison
console.log("abi">"melwin")  // output will be in boolean (true or false)


//ternary operator
// let type = var_name condition X ? "value 1" (if true) : "value 2" (if false)
let age = 9;
let type = age > 18 ? "adult" : "child";
console.log(type)

// logical operation
//types 1.boolean ,2.non-boolean
//1. boolean
//operators(&& (and : both should true),|| (any one can true))
//2.non-boolean
// falsy (undifined , null , 0, false, ->, nan)
// false|| 3 ...... output : 3
//false :(like this out put of sting ,char with compare is same as string , num; for two boolean output will boolean)
//truthy
// anything thats not falsy is truthy

// sample prog for logic operation 
let s= "blue"; // if we put falsy operator insted of blue o/p will yellow
let m= "yellow";
let selected_color = s||m;
console.log(selected_color)


//bitwise operator
console.log(1|2); // "|" bitwise or
console.log(1&2); // "&" bitwise and
// refered in notes
 //example for bitwise

 // write = 0000100
 //read = 0000010
 //execute = 0000001

 let wpermision = 4;
 let rpermesion = 2
 let epermision = 1;
 let my_permision = 0;
let my_permision  =  wpermision | rpermesion
console.log(my_permision)

//operator precedence
// arithemetic operation oredr will in google


