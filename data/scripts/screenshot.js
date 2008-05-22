unbind(KEY_SYSRQ);
bind(KEY_SYSRQ, take_screenshot);

function take_screenshot() {
    filename = "screenshot-" + timestamp() + ".png";
    system.render.takescreenshot(filename);
    print("screenshot saved as: " + filename);
}