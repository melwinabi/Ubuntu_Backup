// for multi user inter face details 
// it is known as methord 
//methords: factory and construction function

function createperson(name){
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