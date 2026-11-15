//PRIMITIVE 

let x=10;
let y =x;
x=20;

//..................
  
//REFERENCE

let z={
    value:30
};

let r =z;
z.value =700;
// value in out can be changed.




// example shopping  with reference


 let cartobj={
    value:9
 };

 function updatecart(cartobj){
    cartobj.value++;
 }

 updatecart(cartobj);
 console.log("carts available",cartobj);