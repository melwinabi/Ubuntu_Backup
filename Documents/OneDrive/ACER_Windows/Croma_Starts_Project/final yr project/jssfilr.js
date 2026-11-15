var firebaseConfig = {
    apiKey: "AIzaSyAe7Sj6Vs3m41hYDyxdhNZREdRniJY4y9I",
    authDomain: "smart-agri-robot.firebaseapp.com",
    databaseURL: "https://smart-agri-robot-default-rtdb.firebaseio.com",
    projectId: "smart-agri-robot",
    storageBucket: "smart-agri-robot.appspot.com",
    messagingSenderId: "966841294563",
    appId: "1:966841294563:web:ef1e132a80c67ca8606fd6",
    measurementId: "G-JQVD2B336R"
  };

  firebase.initializeApp(firebaseConfig);

$(document).ready(function(){
	var database = firebase.database();
	console.log(database,"jjjj")
	var STATUS_A;
	var STATUS_B;
	var STATUS_C;
	var STATUS_D;
	var ON_OFF;
	database.ref().on("value", function(snap){
		STATUS_A = snap.val().STATUS_A;
		STATUS_B = snap.val().STATUS_B;
		STATUS_C = snap.val().STATUS_C;
		STATUS_D = snap.val().STATUS_D;
		ON_OFF   = snap.val().ON_OFF;
		if(STATUS_A == "1"){
			document.getElementById("unact").style.display = "none";
			document.getElementById("act").style.display = "block";
		} else {
			document.getElementById("unact").style.display = "block";
			document.getElementById("act").style.display = "none";
		}
		if(STATUS_B == "1"){
			document.getElementById("unact1").style.display = "none";
			document.getElementById("act1").style.display = "block";
		} else {
			document.getElementById("unact1").style.display = "block";
			document.getElementById("act1").style.display = "none";
		}
		if(STATUS_C == "1"){
			document.getElementById("unact2").style.display = "none";
			document.getElementById("act2").style.display = "block";
		} else {
			document.getElementById("unact2").style.display = "block";
			document.getElementById("act2").style.display = "none";
		}
		if(STATUS_D == "1"){
			document.getElementById("unact3").style.display = "none";
			document.getElementById("act3").style.display = "block";
		} else {
			document.getElementById("unact3").style.display = "block";
			document.getElementById("act3").style.display = "none";
		}

		
		if(ON_OFF == "1"){
			document.getElementById("unact4").style.display = "none";
			document.getElementById("act4").style.display = "block";
		} else {
			document.getElementById("unact4").style.display = "block";
			document.getElementById("act4").style.display = "none";
		}
	});
	$(".toggle-btn").click(function(){
		var firebaseRef = firebase.database().ref().child("STATUS_A");
		if(STATUS_A == "1"){
			firebaseRef.set("0");
			STATUS_A = "0";
			
		} else {
			firebaseRef.set("1");
			STATUS_A = "1";
			
			}
	})
	$(".toggle-btn1").click(function(){
		var firebaseRef = firebase.database().ref().child("STATUS_B");
		if(STATUS_B == "1"){
			firebaseRef.set("0");
			STATUS_B = "0";
		} else {
			firebaseRef.set("1");
			STATUS_B = "1";
		}
	})
	$(".toggle-btn2").click(function(){
		var firebaseRef = firebase.database().ref().child("STATUS_C");
		if(STATUS_C == "1"){
			firebaseRef.set("0");
			STATUS_C = "0";
		} else {
			firebaseRef.set("1");
			STATUS_C = "1";
		}
	})
	$(".toggle-btn3").click(function(){
		var firebaseRef = firebase.database().ref().child("STATUS_D");
		if(STATUS_D == "1"){
			firebaseRef.set("0");
			STATUS_D = "0";
		} else {
			firebaseRef.set("1");
			STATUS_D = "1";
		}
	})
	$(".toggle-btn4").click(function(){
		var firebaseRef = firebase.database().ref().child("ON_OFF");
		if(ON_OFF == "1"){
			firebaseRef.set("0");
			ON_OFF = "0";
		} else {
			firebaseRef.set("1");
			ON_OFF = "1";
		}
	})
});