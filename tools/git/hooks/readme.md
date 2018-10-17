### Git-Hooks

#### Installation
Run ```./install_hooks.sh```

##### Dependencies
* None

#### The Hooks
##### pre-commit
Runs before creating a commit.
Runs quality tests on code (e.g. static analysis)
##### commit-msg
Runs after saving a commit message.
Checks formatting of the commit message.


#### Testing
Run ```py.test``` from the ```.../git/hooks``` directory

##### Dependencies
* ```mock```
* ```pytest```
* ```pytest-cov```
