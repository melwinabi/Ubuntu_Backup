// for multi user inter face details 
// it is known as methord 
//methords: factory and construction function

function createPerson(name){    // camel case
    return{
       name ,  // this line is equal to, name: name,
       greeting: function(){
           let msg = `my name is ${this.name}`;
           console.log(msg);
         }
     };
   }
   let melwin = createperson("melwin");
   melwin.greeting();
   let trishu = createperson("trishu");
   trishu.greeting();




   //// constructor function
 function Person(name) {      //pascal case
 this.name = name; 
 this.greeting= function(){
    console.log (`my name is ${name}`);
 }
}

let person = new person ("abi");
person.greeting();
