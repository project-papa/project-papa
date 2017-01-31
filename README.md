# Project Papa

## Contributing

### Setting up your workspace

To get ready to make contributions, you'll need to have your own version of this repository. We use [Git](https://git-scm.com/) for version control, and we're hosting it on GitHub.

This guide will only have a high-level overview of Git. There are plenty of guides you can follow online to get a better idea. You can also ask someone else on the Papa team for help.

Fork this repository from your account by clicking on the "Fork" button at the top right

With Git installed, clone your fork, and configure the repository

```
git clone https://github.com/<<YOUR USERNAME>>/project-papa
cd project-papa
git remote add upstream https://github.com/project-papa/project-papa
```

You will be making changes to your local fork, pushing them, then opening pull requests against the canonical copy (upstream).

If changes have been made to the canonical copy, you can pull them down at any time:

```
git checkout master
git pull upstream master
```

To get this repository set up with an IDE like Eclipse or IntelliJ IDEA, simply import the folder as a Gradle project.

### Making Changes

To make changes, you want to check out a new branch

```
git checkout -b BRANCH_NAME
```

Then, make changes to your code. Add the feature you plan to add. Please write new tests, and then run them to make sure nothing is broken

```
./gradlew test
```

Then you want to commit your changes, and push them to your fork

```
git add . # Add all changed files
git commit -m "MY MESSAGE" # Make a commit with a message
git push origin HEAD # Push the newly committed branch to your fork
```

After this, you'll be able to make a pull request by going to the repository home page (this page) and clicking the big "Pull Request" button that should appear

### Code Review

Your changes will need to be approved by another member of the team. Nominate one when you're making a pull request.

Smaller changes are easier to review than larger ones, so it is wise to keep pull requests small. It will also help you as a developer to make focused changes.
