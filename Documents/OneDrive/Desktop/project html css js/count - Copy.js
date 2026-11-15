const days = document.querySelector("#days");
const hrs = document.querySelector("#hrs");
const min= document.querySelector("#min");
const sec = document.querySelector("#sec");



function printtm(){
const currentyr= new Date().getFullYear();
const newyr =new Date(`january 1 ${currentyr+1} 00:00:00`);
// console.log(newyr);
const currentdate= new Date();
// console.log(currentdate);
const diff = newyr - currentdate; // ans will be in ms
const d= Math.floor(diff/1000/60/60/24);
// console.log(d,`days`);
const h =  Math.floor((diff/1000/60/60)%24);
// console.log(h,`hours`);
const m =  Math.floor((diff/1000/60)%60);
// console.log(m,`minutes`);
const s =  Math.floor((diff/1000)%60);
// console.log(s,`seconds`);
// 1000ms = 1s;
console.log(`TIMER :` + " "+d+ "  " + h+"  "+ m+ "  " +s+"  " );
 days.innerHTML=d<10?"0"+d:d;
}
printtm();
setInterval(printtm,1000); //to provide refresh for the function new time in sec