//learning arrAY IN DEPTH

//adding an element in array

const cart = ["fruits",`1`,true,undefined,null]

//above are stored as stack

//push
cart.push("hello")
// above store in end

//unshift
cart.unshift("kolloi");
//above will store in front

//splice
cart.splice(2,0,"polloution");

console.log(cart);

// finding an element 
console.log(cart.indexOf("1"));
 //absent element output is : -1.


//condition check
 console.log(cart.indexOf("1")!== -1);
 // truue r false
console.log( cart.includes("banana"))


const  oders = [
    {id: 1, item: "paint", quality:1},
    {id: 2, item: "roolers", quality:1},
]

let result = oders.find (function(order) {
    return order.item == "roolers"

})
console.log(result)

// arrow functions
let result2 = oders.find((order)=>order.item=='paint')
console.log(result2)

//removing an lement
const remover=[1,2,34,4,6,76,7,89,9,8,56,34];
//remover.pop()
remover.splice(3,2)
console.log(remover)

// emptying an array
let empty =[1,32,434,22,332,2];
empty = [];   
console.log(empty); 

// combination and extraction
let shopping = ["hsidi","gfyofgefyg","p0n8y4"];
let shopping2 = ["q34yuhnoq","4qcn","8u3m"];
let all = shopping.concat(shopping2);


//let all2 =all.slice(1,3);
shopping2[1].item ="changed"; // change the values
console.log(all);


//spread operator
// we cant merge the more array using concat hence we move to spread array v
let allo=[...shopping,5454,...shopping2];
console.log(allo);

//itterating an js array
 const dailyRoutine =["wake up","eat","ready"];
for (let routin in dailyRoutine){     //for-of  also used
    console.log(routin,dailyRoutine[routin]);
}
//for each 
//it is impriovement of above both
const life=["uguig","utiut","iygut","iguy","fguyilu"];
life.forEach(function(time){
    console.log(time);
});


//joining and splioting the array
//joining
const stock=["ruyrfuy","87r78t","yfuyf","uyftguytg","9ufg8yig"];
let stocks = stock.join(", "); // symbol in bracket will be displayed in output
console.log(stocks);

//split
let name = "Melwin Abishek";
let div = name.split(" ");
let one = div[0];
let two = div[1];
console.log(`my first name is ${one} and last name is ${two}`);


let postName = "this is my post";
let newName = postName.split(" ");
let newwst = newName.join("_");
console.log(newwst);



//sorting an js array
// sorting by assending is done by normal sort() command.
const student =[7,3,29,12,5,27,9];
student.sort(function(a,b){   //comand for decending
    return b-a;
});
console.log(student);

 //.reverse() also used for left to right display 




 const names =[
    {id:1,name:"ojishjwooj"},
    {id:2,name:"iahi"},
    {id:3,name:"ajdo"},
    {id:4,name:"swkj"},
 ];
 names.sort((a,b)=>{

 //a<b => -1
 if (a.name<b.name) return -1;

 //a>b => 1
 if(a.name>b.name) return 1;

 //id a==b =>0

 if (a.name==b.name) return 0;

});
console.log(names)






//testing of element in js array
//allow only adults abve 20
const byAge=[12,23,45,67,19];
byAges=byAge.every(function(value,index,array){ // some() also used for ony one is true
    // console.log(`value:${value}`);
    // console.log(`index:${index}`);
    // console.log(`array:${array}`);
    return value >= 20

});
console.log("restrcted : ",byAges);



//testing js object
const gagts=[
    {id:1,pdt:"Mobiles", isavailable:true},
    {id:2,pdt:"charger", isavailable:true},
    {id:3,pdt:"battery", isavailable:false},
]; 
//using every and some we can check the stocks of gagts



//filter an js array
const int=[1,2,3,4,5,6,,7,8,9];
let even =int.filter(function(value){
return value % 2 ==0;
});
let odd =int.filter(function(value){
    return value % 2 ==1;
    });

console.log(even);
console.log(odd);


//finding an js array .....find()
// indIndex() shows the place of elemt in array


//map() in js array
const letmaps =[1,2,4,6,8];
console.log(letmaps);
const mapsn =letmaps.map(function(val){
  return val * 2;
});
console.log( mapsn);
 

//recall in array
const dresses =[
    {id:1, title:"shirt", cost:671000},
    {id:2, title:"coat", cost: 99.2000},
    {id:3, title:"saree", cost: 546.3000},
    {id:4, title:"phant", cost: 234000},
];
//sorting in accending
let price=dresses.sort(function(a,b){
    if (a.cost<b.cost) return -1;
    if (a.cost>b.cost) return 1;
    if (a.cost==b.cost) return 0;

    
});
console.log(price);
////filter
const costs = price.filter(function (value){

    return value.cost<100;
});
console.log( costs);


const fee =[123,324,56,78,89,890,455];
const tot = fee.reduce(function(b,a){
    //logic 
     return a + b;
},20); // 20 also added we can use this as initial value 
console.log(tot);
//add all the value and futer value that is being added.



//.............................................END..............................................//














