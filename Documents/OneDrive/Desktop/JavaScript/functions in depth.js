// concepts 
// function decleration , function expression , anonymous function , logingin


//JS hosting
//function decleration 
//function expression 



//self-invoking function (IIFE)
//(run function without calling)
(()=>{
    let num1 = 778655;
    let num2 = 765467;
    let result = num1 + num2;
    (function displayNumber(){
        console.log(result);

    })();
    console.log(" is tha answer")
})();



//function argumrnt
let food = 987;
let drees =7659;
let fuel = 987;
let accomadation = 8790;

function expence(){
    let total = food + fuel + drees + accomadation;
    console.log(`total expence is : ${total}`);
};
expence(); // passing a value to function is arrgument.


//js rest operator
 