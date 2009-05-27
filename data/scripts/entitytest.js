function TestEntity(p) {
    this.something = p;
    this.sayHello = SayHello;
    system.scene.entities.createEntity("ETest" + p, this);
}

function SayHello() {
    print("Hello, this is " + this.name);
}

var etest = new TestEntity("1");