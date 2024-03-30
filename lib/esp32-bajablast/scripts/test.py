Import("env", "projenv")

def post_install_script(source, target, env):
    print("Post-install script running...")
    # Add your custom post-installation code here

# Register the post-install function as an action
env.AddPostAction("$BUILD_DIR", post_install_script)