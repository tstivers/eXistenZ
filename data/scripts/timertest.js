function ttest(c) {    
    print("ttest called from component: " + c.name);   
}

var bleh = system.scene.entities.createEntity("timertest");
var tt = bleh.createTimerComponent("timer");
tt.delay = 1000; // delay 1 second
tt.action = ttest; // call ttest function
tt.start();
