
const clock = document.querySelector('.clock');
function currentTime() {
   var date = new Date(); 
   var hh = date.getHours();
   var mm = date.getMinutes();
   var ss = date.getSeconds();
   var session = "AM";
 
   if(hh > 12){
    hh = hh - 12;
    session = "PM";
   }
   if(hh==0){
      hh=12;
     session="AM";
      }
 
    hh = (hh < 10) ? "0" + hh : hh;
    mm = (mm < 10) ? "0" + mm : mm;
    ss = (ss < 10) ? "0" + ss : ss;
     
    clock.innerHTML =`${hours}:${mm < 10 ? '0' : ''}${mm}:${ss < 10 ? '0' : ''}${ss} ${session}`;
    clockElement.innerText = timeString;
}
   currentTime();
   setInterval(currentTime,1000);