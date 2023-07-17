from datasette.app import Datasette
import pytest


@pytest.mark.asyncio
async def test_plugin_is_installed():
    datasette = Datasette(memory=True)
    response = await datasette.client.get("/-/plugins.json")
    assert response.status_code == 200
    installed_plugins = {p["name"] for p in response.json()}
    assert "datasette-sqlite-assert" in installed_plugins

@pytest.mark.asyncio
async def test_sqlite_assert_functions():
    datasette = Datasette(memory=True)
    response = await datasette.client.get("/_memory.json?sql=select+assert_version(),assert('alex')")
    assert response.status_code == 200
    assert_version, a = response.json()["rows"][0]
    assert assert_version[0] == "v"
    assert len(a) == 26
