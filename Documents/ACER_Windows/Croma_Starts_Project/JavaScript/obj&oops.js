// oops - object will communicate each other in high level application

let person={ 
name :"fgufgjugh",
 age : 9,
 gender :"male",
 addresses : {  // nested object
    city:"uyfg",
    state:"tgvu",
    street:"ufyfgyi",
},
greeting : function (){
    let msg = `my name is ${this.name}, age is ${this.age}`;
     console.log(msg);
}
};
console.log(person.age)
console.log(person.addresses)
//console.log(person.greeting)
