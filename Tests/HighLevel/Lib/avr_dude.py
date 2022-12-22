import subprocess


def flash(name):
    subprocess.run(f"avrdude -p m2560 -c avrisp2 -U flash:w:{name}:i", shell=True, check=True)


def reset_device():
    subprocess.Popen(["avrdude",  "-p", "m2560", "-c", "avrisp2"])
