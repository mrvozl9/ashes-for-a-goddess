# AI-assisted: Codex, 12.11.2026, read-only Unreal MCP identity check. Reviewed by <pending>.
"""Verify this checkout's live Unreal MCP without changing or saving assets."""
import argparse
import json
from pathlib import Path
from urllib.request import Request, build_opener, ProxyHandler


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--url", default="http://127.0.0.1:8001/mcp")
    args = parser.parse_args()
    expected = (Path(__file__).resolve().parent.parent / "Ashes.uproject").as_posix()
    headers = {"Content-Type": "application/json", "Accept": "application/json, text/event-stream"}
    opener = build_opener(ProxyHandler({}))
    sequence = 0

    def rpc(method, params=None, notification=False):
        nonlocal sequence
        sequence += 1
        payload = {"jsonrpc": "2.0", "method": method}
        if not notification:
            payload["id"] = sequence
        if params is not None:
            payload["params"] = params
        request = Request(args.url, data=json.dumps(payload).encode(), headers=headers, method="POST")
        with opener.open(request, timeout=45) as response:
            session = response.headers.get("Mcp-Session-Id")
            if session:
                headers["Mcp-Session-Id"] = session
            body = response.read().decode("utf-8")
        if notification or not body.strip():
            return {}
        if body.lstrip().startswith("{"):
            result = json.loads(body)
        else:
            events = [json.loads(line[5:].strip()) for line in body.splitlines()
                      if line.startswith("data:") and line[5:].strip() not in ("", "[DONE]")]
            result = next((event for event in events if event.get("id") == sequence), None)
            if result is None:
                raise RuntimeError("No matching JSON-RPC response in event stream")
        if "error" in result:
            raise RuntimeError(json.dumps(result["error"]))
        return result.get("result", {})

    initialized = rpc("initialize", {"protocolVersion": "2024-11-05", "capabilities": {},
                                    "clientInfo": {"name": "ashes-codex-check", "version": "1.0"}})
    headers["MCP-Protocol-Version"] = initialized.get("protocolVersion", "2024-11-05")
    rpc("notifications/initialized", notification=True)
    tools = []
    params = {}
    while True:
        page = rpc("tools/list", params)
        tools.extend(page.get("tools", []))
        if not page.get("nextCursor"):
            break
        params = {"cursor": page["nextCursor"]}
    python_tool = next((tool for tool in tools if tool["name"] == "execute_python_code"), None)
    if python_tool is None:
        raise RuntimeError("VibeUE execute_python_code is missing from tools/list")
    # Read project identity first. No saves or asset mutations, even on the wrong server.
    code = chr(10).join([
        "import unreal, json",
        "world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()",
        "environment = unreal.WorkflowService.get_environment()",
        "environment = json.loads(environment) if isinstance(environment, str) else environment",
        "print('ASHES_MCP_IDENTITY=' + json.dumps({'projectFile': unreal.Paths.convert_relative_path_to_full(unreal.Paths.get_project_file_path()), 'map': world.get_path_name(), 'vibeue': hasattr(unreal, 'WorkflowService'), 'environmentProject': environment.get('projectFile')}))",
    ])
    result = rpc("tools/call", {"name": python_tool["name"], "arguments": {"code": code, "auto_save": False}})
    if result.get("isError"):
        raise RuntimeError(json.dumps(result))
    text = "\n".join(item.get("text", "") for item in result.get("content", []))
    # VibeUE wraps stdout in its JSON execution result.
    identity = None
    for block in result.get("content", []):
        if block.get("type") != "text":
            continue
        value = block.get("text", "")
        try:
            execution = json.loads(value)
        except json.JSONDecodeError:
            execution = None
        if isinstance(execution, dict):
            if execution.get("success") is False:
                raise RuntimeError(value)
            value = str(execution.get("output", execution.get("stdout", value)))
        for line in value.splitlines():
            if "ASHES_MCP_IDENTITY=" in line:
                identity = json.loads(line.split("ASHES_MCP_IDENTITY=", 1)[1])
                break
    if identity is None:
        raise RuntimeError("Identity marker missing: " + text[:1500])
    actual = identity["projectFile"].replace(chr(92), "/").rstrip("/")
    if actual.casefold() != expected.casefold():
        raise RuntimeError("Wrong editor: " + actual + "; expected " + expected)
    if not identity["vibeue"]:
        raise RuntimeError("VibeUE WorkflowService unavailable")
    print(json.dumps({"status": "ok", "url": args.url, "server": initialized.get("serverInfo"),
                      "toolCount": len(tools), **identity}, indent=2))


if __name__ == "__main__":
    main()
