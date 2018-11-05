hunter_upload_password(
    # REPO_OWNER + REPO = https://github.com/wheybags/hunter-cache
    REPO_OWNER "wheybags"
    REPO "hunter-cache"

    # USERNAME = https://github.com/freeablo-bot
    USERNAME "freeablo-bot"

    # PASSWORD = GitHub token saved as a secure environment variable
    PASSWORD "$ENV{GITHUB_USER_PASSWORD}"
)