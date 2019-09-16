FreeRTOS memory safety proofs
=

See the README.md in the parent directory for details on how to run the
proofs.

Build system
-

The `prepare.py` script generates a complete, self-contained Makefile in
each directory that contains a proof. Changing into one of those
directories and running `make report` generates a HTML report of the
proof run. The remainder of this section documents the format of the
build manifests.

The Makefile is primarily constructed from `Makefile.template`. However,
additional definitions get added at various points: OS-specific defines
for all proofs, protocol-specific defines, and proof-specific defines.
There are thus JSON files throughout the tree, that contain these
settings. The `prepare.py` script accumulates these settings along a
particular path to a proof, and then dumps a complete Makefile for that
proof in the proof's directory.

There are several different files that are used to generate a proof's Makefile:

* `Makefile.json` contains the settings for a particular proof. If a
  `Makefile.json` exists in a directory, then `prepare.py` will write a
  `Makefile` to that directory.

* `settings.json` has the same format as `Makefile.json`, but can exist
  anywhere in the proof tree (e.g. in a directory containing multiple
  proofs). Lower-down `settings.json` and `Makefile.json` files
  'inherit' settings from `settings.json`.

* `Configurations.json` has the same format as `Makefile.json`, except
  that the value of `"DEF"` is a list of dicts. A proof that has a
  `Configurations.json` file gets run in several configurations, one for
  each of the dicts. When `prepare.py` runs, one subdirectory is created
  for each configuration, and a `Makefile.json` is written to that
  directory, so that `prepare.py` can subsequently transform the
  `Makefile.json` into a `Makefile`.

To summarize the build process:

1. `prepare.py` finds all directories with a `Configurations.json`. It
   creates one subdirectory for each configuration, and writes a
   `Makefile.json` into each of those subdirectories, identical to the
   `Configurations.json` but with only a single dict for the "DEF" key.
2. `prepare.py` walks the directory tree from the `proofs` directory
   downward. It reads all `settings.json` files; deeper settings replace
   shallower ones along a particular path down the tree.
3. Whenever `prepare.py` finds a `Makefile.json`, it reads the file,
   adding its settings to everything found in the `settings.json` files,
   and additionally writes a `Makefile` to that directory.

`settings.json` has the following structure:

```python
    {
      # These variables eventually become Makefile variables. The values
      # of variables can reference previously-defined variables, using
      # curly-brackets like {var_name}. The values can also evaluate
      # basic arithmetic by including "__eval" at the beginning of the
      # variable's value. See proof_specific_makefile.py for details on
      # what arithmetic is allowed.
      "variables": dict(str -> str),

      # Each of these lists has special semantics. "INC" is the list of
      # files to include for building a proof; "DEF" is the list of
      # command-line defines needed to build a particular proof, etc.
      "lists": dict(str -> str list),

      # These flags change the actual rules that get written out to the
      # Makefile, as opposed to changing the defines or the targets.
      "makefile-control": dict(str -> ??),
    }
```

When a `settings.json` file is read, it overwrites previous
`settings.json`s that were read from a shallower directory as follows:

* Values under the `"variables"` key get overwritten with the deeper
  value;
* Items in a list under `"lists"` get *appended* to that list; the list
  does not get overwritten;
* Values under `"makefile-control"` get overwritten.

This allows for a common list of include files, which gets appended to
for particular proofs, for example.

Variables and arithmetic do not get evaluated until just before the
`Makefile` gets written out. In other words, suppose we have a top-level
`settings.json`:

```json
{
  "variables": {
    "foo": "qux"
  }
}
```

and another `settings.json` in a deeper directory:

```json
{
  "variables": {
    "foo": "7",
    "bar": "__eval 0 if {foo} < 0 else {foo}"
  },
  "lists": {
    "DEF": [
      "BAR={bar}"
    ]
  }
}
```

And finally a `Makefile.json` in a directory beneath that:

```json
{
  "variables": {
    "baz": "__eval {bar} + 10"
  },
  "lists": {
    "DEF": [
      "BAZ={baz}{bar}"
    ]
  }
}
```

The generated Makefile would have the following definitions:

```Makefile
foo = 7
bar = 7
baz = 17

DEF = \
  -DBAR=7 \
  -DBAZ=177 \
  # empty
```
