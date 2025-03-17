Import("env")
import time

def after_upload(source, target, env):
    active_env = env["PIOENV"]
    print(f"Uploading SPIFFS/LittleFS for {active_env}...")
    time.sleep(2)
    env.Execute(f"pio run -e {active_env} -t uploadfs")

env.AddPostAction("upload", after_upload)
