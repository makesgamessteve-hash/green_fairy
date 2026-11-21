"""End-to-end checks against the served Emscripten distribution."""
import argparse
import base64
import functools
import http.server
import shutil
import tempfile
import threading
from pathlib import Path

from playwright.sync_api import sync_playwright


class Handler(http.server.SimpleHTTPRequestHandler):
    def log_message(self, *_):
        pass


def point(page, x, y, width=1000, height=740):
    bounds = page.locator("#canvas").bounding_box()
    zoom = min(bounds["width"] / width, bounds["height"] / height)
    return (
        bounds["x"] + (bounds["width"] - width * zoom) / 2 + x * zoom,
        bounds["y"] + (bounds["height"] - height * zoom) / 2 + y * zoom,
    )


def load(page, url):
    errors = []
    page.on("pageerror", lambda error: errors.append(str(error)))
    page.goto(url, wait_until="networkidle")
    page.wait_for_function("window.Module?._gf_web_ready?.() === 1")
    page.locator("#status").wait_for(state="hidden")
    assert not errors, errors
    return errors


def scene_colors(page):
    x, y = point(page, 230, 151)
    right, bottom = point(page, 470, 332)
    capture = page.screenshot(clip={"x": x, "y": y, "width": right - x, "height": bottom - y})
    return page.evaluate("""async png => {
        const image = new Image();
        image.src = 'data:image/png;base64,' + png;
        await image.decode();
        const canvas = document.createElement('canvas');
        canvas.width = image.width;
        canvas.height = image.height;
        const context = canvas.getContext('2d');
        context.drawImage(image, 0, 0);
        const pixels = context.getImageData(0, 0, canvas.width, canvas.height).data;
        let green = 0, violet = 0;
        for (let i = 0; i < pixels.length; i += 4) {
            const r = pixels[i], g = pixels[i + 1], b = pixels[i + 2];
            if (g > 70 && g > r * 1.1 && g > b * 1.2) green++;
            if (r > 75 && b > 75 && r > g * 1.16 && b > g * 1.25) violet++;
        }
        return [green, violet];
    }""", base64.b64encode(capture).decode("ascii"))


def desktop(browser, url, screenshots):
    page = browser.new_page(viewport={"width": 1160, "height": 834})
    requests = set()
    page.on("response", lambda response: requests.add(response.url) if response.ok else None)
    errors = load(page, url)
    for path in ("manifest.json", "scripts/bank.lua", "sounds/canopy.wav",
                 "graphics/studio.bmp", "graphics/prism.obj"):
        assert url + "assets/" + path in requests, path
    assert sum(request.endswith(".obj") for request in requests) == 11
    assert page.evaluate("Module._gf_web_touch()") == 0
    assert page.evaluate("Module._gf_web_patch()") == 0
    assert .35 < page.evaluate("Module._gf_web_control(0)") < .37
    png = page.locator("#canvas").screenshot(path=screenshots / "desktop.png" if screenshots else None)
    assert len(png) > 15000, "The instrument canvas is unexpectedly blank"
    green, violet = scene_colors(page)
    assert green > 500 and green > 4 * violet, (green, violet)
    page.mouse.click(*point(page, 574, 581))
    page.keyboard.down("a")
    page.keyboard.down("s")
    page.wait_for_function("Module._gf_web_held() === 2 && Module._gf_web_peak() > .015")
    page.keyboard.up("a")
    page.keyboard.up("s")
    page.wait_for_function("Module._gf_web_held() === 0")

    page.keyboard.press("PageUp")
    page.wait_for_function("Module._gf_web_patch() === 90")
    page.keyboard.press("PageDown")
    page.wait_for_function("Module._gf_web_patch() === 0")
    page.keyboard.press("ArrowUp")
    page.wait_for_function("Module._gf_web_patch() === 99")
    green, violet = scene_colors(page)
    assert violet > 500 and violet > 4 * green, (green, violet)
    if screenshots:
        page.locator("#canvas").screenshot(path=screenshots / "blacklight.png")
    page.keyboard.down("a")
    page.wait_for_function("Module._gf_web_peak() > .012")
    page.keyboard.up("a")
    page.keyboard.press("ArrowDown")
    page.wait_for_function("Module._gf_web_patch() === 0")

    page.mouse.click(*point(page, 888, 407))
    page.wait_for_function("Module._gf_web_patch() === 1")
    page.mouse.click(*point(page, 805, 286))
    page.wait_for_function("Module._gf_web_patch() === 2")
    page.mouse.click(*point(page, 932, 407))
    page.wait_for_function("Module._gf_web_patch() === 12")
    page.mouse.click(*point(page, 713, 407))
    page.wait_for_function("Module._gf_web_patch() === 2")
    page.mouse.click(*point(page, 295, 526))
    page.wait_for_function("Module._gf_web_control(0) > .90")
    page.mouse.move(*point(page, 92, 676))
    page.mouse.down()
    page.wait_for_function("Module._gf_web_held() === 1")
    page.mouse.move(*point(page, 220, 676))
    page.wait_for_function("Module._gf_web_held() === 1")
    page.mouse.up()
    page.wait_for_function("Module._gf_web_held() === 0")

    page.evaluate("""() => {
        window.contextTest = document.querySelector("#canvas").getContext("webgl2")
            .getExtension("WEBGL_lose_context");
        window.contextTest.loseContext();
    }""")
    page.locator("#status").wait_for(state="visible")
    page.evaluate("window.contextTest.restoreContext()")
    page.locator("#status").wait_for(state="hidden")
    assert len(page.locator("#canvas").screenshot()) > 15000
    assert not errors, errors
    page.close()


def mobile(browser, url, screenshots):
    page = browser.new_page(viewport={"width": 390, "height": 844}, device_scale_factor=2,
                            has_touch=True, is_mobile=True)
    errors = load(page, url)
    assert page.evaluate("Module._gf_web_touch()") == 1
    if screenshots:
        page.screenshot(path=screenshots / "mobile.png")
    page.touchscreen.tap(*point(page, 368, 372, 420))
    page.wait_for_function("Module._gf_web_patch() === 10")
    page.touchscreen.tap(*point(page, 52, 372, 420))
    page.wait_for_function("Module._gf_web_patch() === 0")
    page.touchscreen.tap(*point(page, 310, 372, 420))
    page.wait_for_function("Module._gf_web_patch() === 1")

    cdp = page.context.new_cdp_session(page)
    x, y = point(page, 44, 667, 420)
    cdp.send("Input.dispatchTouchEvent", {
        "type": "touchStart", "touchPoints": [{"id": 1, "x": x, "y": y}]
    })
    page.wait_for_function("Module._gf_web_held() === 1 && Module._gf_web_peak() > .012")
    cdp.send("Input.dispatchTouchEvent", {"type": "touchEnd", "touchPoints": []})
    page.wait_for_function("Module._gf_web_held() === 0")
    page.set_viewport_size({"width": 844, "height": 390})
    page.wait_for_function("document.querySelector('#canvas').width === " +
                           "document.querySelector('#canvas').clientWidth * devicePixelRatio")
    page.touchscreen.tap(*point(page, 721, 137, 820, 454))
    assert page.evaluate("Module._gf_web_touch()") == 1
    page.wait_for_function("Module._gf_web_patch() === 2")
    assert not errors, errors
    page.close()


def external_assets(browser, url):
    page = browser.new_page()
    page.route("**/assets/scripts/bank.lua", lambda route: route.fulfill(
        content_type="text/plain",
        body='return {{name="Browser edit", morph=.91, scene={mesh="missing.obj"}}}'))
    load(page, url)
    page.wait_for_function("Module._gf_web_control(0) > .90")
    green, violet = scene_colors(page)
    assert green > 500 and green > 4 * violet, (green, violet)
    page.close()

    page = browser.new_page()
    page.route("**/assets/graphics/studio.bmp", lambda route: route.fulfill(status=404, body=""))
    page.goto(url)
    page.wait_for_function("document.querySelector('#status').textContent.includes('studio.bmp returned HTTP 404')")
    assert page.locator("#status").is_visible()
    page.close()


def xr(browser, url):
    page = browser.new_page()
    page.add_init_script(path=Path(__file__).with_name("mock-xr.js"))
    errors = load(page, url)
    page.get_by_role("button", name="Enter VR").click()
    page.get_by_role("button", name="Exit VR").wait_for()
    page.wait_for_function("testXR.eyeColors().every(colors => colors > 3)")
    page.evaluate("testXR.select(true)")
    page.wait_for_function("Module._gf_web_held() === 1 && Module._gf_web_peak() > .01")
    page.evaluate("testXR.select(false)")
    page.wait_for_function("Module._gf_web_held() === 0")
    page.get_by_role("button", name="Exit VR").click()
    page.get_by_role("button", name="Enter VR").wait_for()
    page.keyboard.down("a")
    page.wait_for_function("Module._gf_web_held() === 1")
    page.keyboard.up("a")
    assert not errors, errors
    page.close()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("directory", type=Path)
    parser.add_argument("--chromium")
    parser.add_argument("--screenshots", type=Path)
    args = parser.parse_args()
    if args.screenshots:
        args.screenshots.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory() as site_root:
        shutil.copytree(args.directory, Path(site_root) / "green-fairy")
        test_project_site(site_root, args.chromium, args.screenshots)


def test_project_site(site_root, chromium, screenshots):
    handler = functools.partial(Handler, directory=site_root)
    with http.server.ThreadingHTTPServer(("127.0.0.1", 0), handler) as server, sync_playwright() as pw:
        threading.Thread(target=server.serve_forever, daemon=True).start()
        browser = pw.chromium.launch(
            executable_path=chromium,
            args=["--use-angle=swiftshader", "--enable-unsafe-swiftshader"],
        )
        url = f"http://127.0.0.1:{server.server_port}/green-fairy/"
        try:
            desktop(browser, url, screenshots)
            print("Desktop browser passed")
            mobile(browser, url, screenshots)
            print("Mobile touch passed")
            external_assets(browser, url)
            print("External assets passed")
            xr(browser, url)
            print("Simulated WebXR passed")
        finally:
            browser.close()
            server.shutdown()


if __name__ == "__main__":
    main()
