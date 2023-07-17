# The `datasette-sqlite-assert` Datasette Plugin

`datasette-sqlite-assert` is a [Datasette plugin](https://docs.datasette.io/en/stable/plugins.html) that loads the [`sqlite-assert`](https://github.com/asg017/sqlite-assert) extension in Datasette instances.

```
datasette install datasette-sqlite-assert
```

See [`docs.md`](../../docs.md) for a full API reference for the assert SQL functions.

Alternatively, when publishing Datasette instances, you can use the `--install` option to install the plugin.

```
datasette publish cloudrun data.db --service=my-service --install=datasette-sqlite-assert

```
