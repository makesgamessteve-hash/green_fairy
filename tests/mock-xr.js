(() => {
    const identity = new Float32Array([1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1]);
    const projection = identity.slice();
    projection[10] = .4;

    const input = {targetRaySpace: {}};
    const transform = {
        position: {x: -.71 + 1.42 * .09, y: 1.42 * .74 * (.5 - .90) - .04, z: 0},
        orientation: {x: 0, y: 0, z: 0, w: 1}
    };
    const views = [0, 1].map(eye => ({
        eye, projectionMatrix: projection, transform: {inverse: {matrix: identity}}
    }));
    const frame = {
        getViewerPose: () => ({views}),
        getPose: () => ({transform})
    };

    class Session extends EventTarget {
        inputSources = [input];
        ended = false;
        requestReferenceSpace() { return Promise.resolve({}); }
        updateRenderState({baseLayer}) { this.layer = baseLayer; }
        requestAnimationFrame(callback) {
            return requestAnimationFrame(time => { if (!this.ended) callback(time, frame); });
        }
        async end() {
            this.ended = true;
            this.dispatchEvent(new Event("end"));
        }
    }

    let session;
    Object.defineProperty(navigator, "xr", {value: {
        isSessionSupported: async mode => mode === "immersive-vr",
        requestSession: async () => (session = new Session())
    }});
    WebGL2RenderingContext.prototype.makeXRCompatible = async () => {};
    window.XRWebGLLayer = class {
        constructor(_, gl) {
            this.gl = gl;
            this.framebuffer = gl.createFramebuffer();
            const color = gl.createTexture();
            gl.bindTexture(gl.TEXTURE_2D, color);
            gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA8, 640, 320, 0, gl.RGBA, gl.UNSIGNED_BYTE, null);
            gl.bindFramebuffer(gl.FRAMEBUFFER, this.framebuffer);
            gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, color, 0);
            gl.bindFramebuffer(gl.FRAMEBUFFER, null);
        }
        getViewport(view) { return {x: 320 * view.eye, y: 0, width: 320, height: 320}; }
    };

    window.testXR = {
        select(pressed) {
            const event = new Event(pressed ? "selectstart" : "selectend");
            session.dispatchEvent(Object.assign(event, {inputSource: input, frame}));
        },
        eyeColors() {
            const {gl, framebuffer} = session.layer;
            gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);
            const rgba = new Uint8Array(4);
            const eyes = [0, 320].map(offset => {
                const colors = new Set();
                for (let y = 40; y < 280; y += 30) {
                    for (let x = 40; x < 280; x += 30) {
                        gl.readPixels(x + offset, y, 1, 1, gl.RGBA, gl.UNSIGNED_BYTE, rgba);
                        colors.add(rgba.join());
                    }
                }
                return colors.size;
            });
            gl.bindFramebuffer(gl.FRAMEBUFFER, null);
            return eyes;
        }
    };
})();
