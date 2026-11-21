"use strict";

const canvas = document.getElementById("canvas");
const studio = document.getElementById("studio");
const statusMessage = document.getElementById("status");
const xrButton = document.getElementById("xr");
const coarsePointer = matchMedia("(pointer: coarse)");
let startupError;

async function getAsset(path) {
    const response = await fetch(new URL(`assets/${path}`, document.baseURI), {cache: "no-store"});
    if (!response.ok) throw new Error(`${path} returned HTTP ${response.status}`);
    return response;
}

async function mountAssets() {
    const manifest = await (await getAsset("manifest.json")).json();
    if (!Array.isArray(manifest)) throw new Error("Invalid asset manifest");
    const mounted = await Promise.allSettled(manifest.map(async path => {
        if (typeof path !== "string" || !/^(graphics|scripts|sounds)\/[\w.-]+$/.test(path) ||
            path.endsWith("/.") || path.endsWith("/..")) {
            throw new Error("Invalid path in asset manifest");
        }
        const data = await (await getAsset(path)).arrayBuffer();
        const target = `/assets/${path}`;
        Module.FS.mkdirTree(target.slice(0, target.lastIndexOf("/")));
        Module.FS.writeFile(target, new Uint8Array(data));
    }));
    const failure = mounted.find(result => result.status === "rejected");
    if (failure) throw failure.reason;
}

function resizeInstrument() {
    if (!Module._gf_web_ready?.()) return;
    Module._gf_web_resize(canvas.clientWidth, canvas.clientHeight,
                          Number(coarsePointer.matches || navigator.maxTouchPoints > 0));
}

var Module = {
    canvas,
    preRun: [() => {
        Module.addRunDependency("external-assets");
        mountAssets().catch(error => {
            startupError = `Could not load Green Fairy assets. ${error.message}`;
            statusMessage.textContent = startupError;
        }).finally(() => Module.removeRunDependency("external-assets"));
    }],
    printErr(...message) {
        const description = message.join(" ");
        if (description.includes("Green Fairy startup failed")) startupError ||= description;
        console.error(...message);
    },
    onAbort(reason) {
        statusMessage.hidden = false;
        statusMessage.textContent = startupError || `Green Fairy could not start. ${reason}`;
    },
    onRuntimeInitialized() {
        requestAnimationFrame(() => {
            if (!Module._gf_web_ready()) {
                statusMessage.textContent = startupError || "Green Fairy could not initialize WebGL 2.";
                return;
            }
            statusMessage.hidden = true;
            new ResizeObserver(resizeInstrument).observe(canvas);
            coarsePointer.addEventListener("change", resizeInstrument);
            resizeInstrument();
            setupXR();
        });
    }
};

canvas.addEventListener("pointerdown", () => canvas.focus({preventScroll: true}));
canvas.addEventListener("contextmenu", event => event.preventDefault());
canvas.addEventListener("webglcontextlost", event => {
    event.preventDefault();
    Module._gf_web_context_lost?.();
    statusMessage.hidden = false;
    statusMessage.textContent = "Restoring the graphics context…";
});
canvas.addEventListener("webglcontextrestored", () => {
    statusMessage.hidden = Boolean(Module._gf_web_context_restored?.());
    if (!statusMessage.hidden) statusMessage.textContent = "Refresh to restore the graphics context.";
});
addEventListener("blur", () => Module._gf_web_release?.());
document.addEventListener("visibilitychange", () => {
    if (document.hidden) Module._gf_web_release?.();
});

const panelWidth = 1.42;
const panelHeight = panelWidth * .74;
const panelLeft = -panelWidth / 2;
const panelTop = panelHeight / 2 - .04;
const panelDepth = -1.52;
const panelModel = new Float32Array([
    panelWidth / 1000, 0, 0, 0,
    0, -panelHeight / 740, 0, 0,
    0, 0, 1, 0,
    panelLeft, panelTop, panelDepth, 1
]);

function multiply(out, a, b) {
    for (let column = 0; column < 4; ++column) {
        for (let row = 0; row < 4; ++row) {
            out[column * 4 + row] =
                a[row] * b[column * 4] + a[row + 4] * b[column * 4 + 1] +
                a[row + 8] * b[column * 4 + 2] + a[row + 12] * b[column * 4 + 3];
        }
    }
}

function controllerHit(frame, source, space) {
    const pose = frame.getPose(source.targetRaySpace, space);
    if (!pose) return null;
    const {position, orientation} = pose.transform;
    const {x, y, z, w} = orientation;
    const dx = -2 * (x * z + w * y);
    const dy = -2 * (y * z - w * x);
    const dz = 2 * (x * x + y * y) - 1;
    if (Math.abs(dz) < .001) return null;
    const distance = (panelDepth - position.z) / dz;
    if (distance <= 0) return null;
    return {
        x: (position.x + dx * distance - panelLeft) / panelWidth,
        y: (panelTop - position.y - dy * distance) / panelHeight
    };
}

function runXR(session, space, layer, gl) {
    const viewProjection = new Float32Array(16);
    const matrix = new Float32Array(16);
    const matrixAddress = Module._gf_xr_matrix() / Float32Array.BYTES_PER_ELEMENT;
    const contacts = new Map();

    function getContact(source) {
        if (contacts.has(source)) return contacts.get(source);
        const assigned = new Set(Array.from(contacts.values(), contact => contact.id));
        for (let id = 0; id < 16; ++id) {
            if (assigned.has(id)) continue;
            const contact = {id, active: false};
            contacts.set(source, contact);
            return contact;
        }
        return null;
    }

    function release(contact) {
        if (contact?.active) Module._gf_xr_pointer(contact.id, 2, 0, 0);
        if (contact) contact.active = false;
    }

    session.addEventListener("selectstart", event => {
        const contact = getContact(event.inputSource);
        const hit = controllerHit(event.frame, event.inputSource, space);
        if (!contact || !hit) return;
        contact.active = true;
        Module._gf_xr_pointer(contact.id, 0, hit.x, hit.y);
    });
    session.addEventListener("selectend", event => release(contacts.get(event.inputSource)));
    session.addEventListener("inputsourceschange", event => {
        for (const source of event.removed) {
            const contact = contacts.get(source);
            if (contact) Module._gf_xr_cursor(contact.id, 0, 0, 0);
            release(contact);
            contacts.delete(source);
        }
    });

    function render(_, frame) {
        session.requestAnimationFrame(render);
        for (const source of session.inputSources) {
            const contact = getContact(source);
            if (!contact) continue;
            const hit = controllerHit(frame, source, space);
            const visible = hit && hit.x >= 0 && hit.x <= 1 && hit.y >= 0 && hit.y <= 1;
            Module._gf_xr_cursor(contact.id, hit?.x ?? 0, hit?.y ?? 0, Number(Boolean(visible)));
            if (hit && contact.active) Module._gf_xr_pointer(contact.id, 1, hit.x, hit.y);
            if (!hit) release(contact);
        }
        const viewer = frame.getViewerPose(space);
        if (!viewer) return;
        gl.bindFramebuffer(gl.FRAMEBUFFER, layer.framebuffer);
        for (const view of viewer.views) {
            const viewport = layer.getViewport(view);
            multiply(viewProjection, view.projectionMatrix, view.transform.inverse.matrix);
            multiply(matrix, viewProjection, panelModel);
            Module.HEAPF32.set(matrix, matrixAddress);
            Module._gf_xr_draw(viewport.x, viewport.y, viewport.width, viewport.height);
        }
        gl.bindFramebuffer(gl.FRAMEBUFFER, null);
    }
    Module._gf_xr_mode(1);
    session.requestAnimationFrame(render);
}

async function setupXR() {
    if (!navigator.xr || !await navigator.xr.isSessionSupported("immersive-vr").catch(() => false)) return;
    studio.classList.add("xr-available");
    xrButton.hidden = false;
    let session = null;
    xrButton.addEventListener("click", async () => {
        xrButton.disabled = true;
        try {
            if (session) {
                await session.end();
                return;
            }
            session = await navigator.xr.requestSession("immersive-vr");
            session.addEventListener("end", () => {
                session = null;
                Module._gf_xr_mode(0);
                xrButton.textContent = "Enter VR";
                resizeInstrument();
            }, {once: true});
            const gl = canvas.getContext("webgl2");
            if (!gl) throw new Error("WebGL 2 is unavailable");
            await gl.makeXRCompatible();
            const layer = new XRWebGLLayer(session, gl);
            session.updateRenderState({baseLayer: layer});
            const space = await session.requestReferenceSpace("local");
            runXR(session, space, layer, gl);
            statusMessage.hidden = true;
            xrButton.textContent = "Exit VR";
        } catch (error) {
            if (session) await session.end().catch(() => {});
            statusMessage.hidden = false;
            statusMessage.textContent = `Could not start VR. ${error.message}`;
        } finally {
            xrButton.disabled = false;
        }
    });
}
