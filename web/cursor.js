/* ============================================================
   CANVAS CURSOR ANIMATION
   Flowing ribbon trails that follow the mouse
   Converted from React hook to vanilla JS
   ============================================================ */

(function () {
    'use strict';

    // ---- Oscillator class for hue cycling ----
    function Oscillator(config) {
        config = config || {};
        this.phase = config.phase || 0;
        this.offset = config.offset || 0;
        this.frequency = config.frequency || 0.001;
        this.amplitude = config.amplitude || 1;
        this._value = 0;
    }

    Oscillator.prototype.update = function () {
        this.phase += this.frequency;
        this._value = this.offset + Math.sin(this.phase) * this.amplitude;
        return this._value;
    };

    Oscillator.prototype.value = function () {
        return this._value;
    };

    // ---- Node class ----
    function Node() {
        this.x = 0;
        this.y = 0;
        this.vx = 0;
        this.vy = 0;
    }

    // ---- Config ----
    var E = {
        friction: 0.5,
        trails: 20,
        size: 50,
        dampening: 0.25,
        tension: 0.98
    };

    var ctx;
    var oscillator;
    var pos = { x: 0, y: 0 };
    var lines = [];
    var animating = false;
    var canvasEl;

    // ---- Line class ----
    function Line(config) {
        config = config || {};
        this.spring = config.spring + 0.1 * Math.random() - 0.02;
        this.friction = E.friction + 0.01 * Math.random() - 0.002;
        this.nodes = [];
        for (var i = 0; i < E.size; i++) {
            var node = new Node();
            node.x = pos.x;
            node.y = pos.y;
            this.nodes.push(node);
        }
    }

    Line.prototype.update = function () {
        var spring = this.spring;
        var head = this.nodes[0];

        head.vx += (pos.x - head.x) * spring;
        head.vy += (pos.y - head.y) * spring;

        for (var i = 0; i < this.nodes.length; i++) {
            var node = this.nodes[i];
            if (i > 0) {
                var prev = this.nodes[i - 1];
                node.vx += (prev.x - node.x) * spring;
                node.vy += (prev.y - node.y) * spring;
                node.vx += prev.vx * E.dampening;
                node.vy += prev.vy * E.dampening;
            }
            node.vx *= this.friction;
            node.vy *= this.friction;
            node.x += node.vx;
            node.y += node.vy;
            spring *= E.tension;
        }
    };

    Line.prototype.draw = function () {
        var firstX = this.nodes[0].x;
        var firstY = this.nodes[0].y;

        ctx.beginPath();
        ctx.moveTo(firstX, firstY);

        var a, b;
        for (var i = 1, len = this.nodes.length - 2; i < len; i++) {
            a = this.nodes[i];
            b = this.nodes[i + 1];
            var mx = 0.5 * (a.x + b.x);
            var my = 0.5 * (a.y + b.y);
            ctx.quadraticCurveTo(a.x, a.y, mx, my);
        }
        a = this.nodes[i];
        b = this.nodes[i + 1];
        ctx.quadraticCurveTo(a.x, a.y, b.x, b.y);
        ctx.stroke();
        ctx.closePath();
    };

    // ---- Init lines ----
    function initLines() {
        lines = [];
        for (var i = 0; i < E.trails; i++) {
            lines.push(new Line({ spring: 0.4 + (i / E.trails) * 0.025 }));
        }
    }

    // ---- Mouse / Touch handlers ----
    function onMove(e) {
        if (e.touches) {
            pos.x = e.touches[0].pageX;
            pos.y = e.touches[0].pageY;
        } else {
            pos.x = e.clientX;
            pos.y = e.clientY;
        }
    }

    function onFirstInteraction(e) {
        // Remove initial listeners
        document.removeEventListener('mousemove', onFirstInteraction);
        document.removeEventListener('touchstart', onFirstInteraction);

        // Add proper movement listeners
        document.addEventListener('mousemove', onMove);
        document.addEventListener('touchmove', onMove);
        document.addEventListener('touchstart', function (e) {
            if (e.touches.length === 1) {
                pos.x = e.touches[0].pageX;
                pos.y = e.touches[0].pageY;
            }
        });

        // Set initial pos
        onMove(e);

        // Init and start
        initLines();
        render();
    }

    // ---- Render loop ----
    function render() {
        if (!animating) return;

        ctx.globalCompositeOperation = 'source-over';
        ctx.clearRect(0, 0, canvasEl.width, canvasEl.height);
        ctx.globalCompositeOperation = 'lighter';
        ctx.strokeStyle = 'hsla(' + Math.round(oscillator.update()) + ',50%,50%,0.2)';
        ctx.lineWidth = 1;

        for (var i = 0; i < E.trails; i++) {
            lines[i].update();
            lines[i].draw();
        }

        window.requestAnimationFrame(render);
    }

    // ---- Resize ----
    function resizeCanvas() {
        canvasEl.width = window.innerWidth;
        canvasEl.height = window.innerHeight;
    }

    // ---- Public init ----
    function initCanvasCursor() {
        canvasEl = document.getElementById('cursorCanvas');
        if (!canvasEl) return;

        ctx = canvasEl.getContext('2d');
        animating = true;

        oscillator = new Oscillator({
            phase: Math.random() * 2 * Math.PI,
            amplitude: 85,
            frequency: 0.0015,
            offset: 285
        });

        document.addEventListener('mousemove', onFirstInteraction);
        document.addEventListener('touchstart', onFirstInteraction);

        window.addEventListener('resize', resizeCanvas);
        document.body.addEventListener('orientationchange', resizeCanvas);

        window.addEventListener('focus', function () {
            if (!animating) {
                animating = true;
                render();
            }
        });

        window.addEventListener('blur', function () {
            // Keep animating on blur for smoother experience
            animating = true;
        });

        resizeCanvas();
    }

    // ---- Auto-init on DOMContentLoaded ----
    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', initCanvasCursor);
    } else {
        initCanvasCursor();
    }
})();
