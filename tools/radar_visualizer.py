#!/usr/bin/env python3
"""High resolution serial radar visualizer for the ultrasonic radar firmware."""

from __future__ import annotations

import argparse
import math
import queue
import re
import sys
import threading
import time
from dataclasses import dataclass
from typing import Iterable

import pygame
import serial
from serial.tools import list_ports


SAMPLE_RE = re.compile(r"^\$?A(?P<angle>\d+),D(?P<distance>\d+),I(?P<index>\d+)$", re.I)
CONFIG_RE = re.compile(r"^\$(?P<key>[a-z_]+)\s*[:= ]\s*(?P<value>-?\d+)\s*$", re.I)
USART_ENABLE_BYTE = b"y"
SERIAL_RESET_DELAY_SECONDS = 2.0

KEY_ALIASES = {
    "mind": "min_distance",
    "min_distance": "min_distance",
    "min_dist": "min_distance",
    "maxd": "max_distance",
    "max_distance": "max_distance",
    "max_dist": "max_distance",
    "mina": "min_angle",
    "min_angle": "min_angle",
    "maxa": "max_angle",
    "max_angle": "max_angle",
    "speed": "scan_speed",
    "scan_speed": "scan_speed",
}


@dataclass
class RadarLimits:
    min_angle: int = 0
    max_angle: int = 180
    min_distance: int = 20
    max_distance: int = 200
    scan_speed: int = 99

    def apply(self, key: str, value: int) -> None:
        attr = KEY_ALIASES.get(key.lower())
        if not attr:
            return
        setattr(self, attr, value)
        self.min_angle = max(0, min(180, self.min_angle))
        self.max_angle = max(0, min(180, self.max_angle))
        self.min_distance = max(0, self.min_distance)
        self.max_distance = max(self.min_distance + 1, self.max_distance)


@dataclass
class RadarSample:
    angle: int
    distance: int
    index: int
    received_at: float


def parse_line(line: str) -> tuple[str, object] | None:
    text = line.strip()
    if not text:
        return None

    sample = SAMPLE_RE.match(text)
    if sample:
        return (
            "sample",
            RadarSample(
                angle=int(sample.group("angle")),
                distance=int(sample.group("distance")),
                index=int(sample.group("index")),
                received_at=time.monotonic(),
            ),
        )

    if text.startswith("$"):
        config = CONFIG_RE.match(text)
        if config:
            return "config", (config.group("key").lower(), int(config.group("value")))
        return "status", text[1:]

    return "status", text


class SerialReader(threading.Thread):
    def __init__(self, port: str, baud: int, out: queue.Queue[tuple[str, object]]) -> None:
        super().__init__(daemon=True)
        self.port = port
        self.baud = baud
        self.out = out
        self.stop_event = threading.Event()

    def run(self) -> None:
        try:
            with serial.Serial(self.port, self.baud, timeout=0.2) as conn:
                time.sleep(SERIAL_RESET_DELAY_SECONDS)
                conn.reset_input_buffer()
                conn.write(USART_ENABLE_BYTE)
                conn.flush()
                self.out.put(("status", "sent USART rendering enable byte"))
                while not self.stop_event.is_set():
                    raw = conn.readline()
                    if not raw:
                        continue
                    try:
                        line = raw.decode("ascii", errors="replace")
                    except UnicodeDecodeError:
                        continue
                    parsed = parse_line(line)
                    if parsed:
                        self.out.put(parsed)
        except serial.SerialException as exc:
            self.out.put(("error", str(exc)))

    def stop(self) -> None:
        self.stop_event.set()


class RadarView:
    def __init__(self, port: str, baud: int) -> None:
        pygame.init()
        pygame.display.set_caption("Ultrasonic Radar Visualizer")
        self.screen = pygame.display.set_mode((1100, 760), pygame.RESIZABLE)
        self.clock = pygame.time.Clock()
        self.font = pygame.font.SysFont("Consolas", 17)
        self.small_font = pygame.font.SysFont("Consolas", 14)

        self.limits = RadarLimits()
        self.samples: dict[int, RadarSample] = {}
        self.current_angle: int | None = None
        self.receiving_config = False
        self.last_status = "Waiting for serial data"
        self.last_error = ""
        self.port = port
        self.baud = baud
        self.messages: queue.Queue[tuple[str, object]] = queue.Queue()
        self.reader = SerialReader(port, baud, self.messages)

    def run(self) -> None:
        self.reader.start()
        running = True
        while running:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False
                elif event.type == pygame.KEYDOWN and event.key in (pygame.K_ESCAPE, pygame.K_q):
                    running = False

            self._drain_messages()
            self._draw()
            self.clock.tick(60)

        self.reader.stop()
        pygame.quit()

    def _drain_messages(self) -> None:
        while True:
            try:
                kind, payload = self.messages.get_nowait()
            except queue.Empty:
                return

            if kind == "sample":
                sample = payload
                assert isinstance(sample, RadarSample)
                self.receiving_config = False
                self.samples[sample.index] = sample
                self.current_angle = sample.angle
            elif kind == "config":
                key, value = payload
                if not self.receiving_config:
                    self._clear_radar()
                    self.receiving_config = True
                self.limits.apply(key, value)
                self.last_status = f"{key} = {value}"
            elif kind == "error":
                self.last_error = str(payload)
            elif kind == "status":
                self.last_status = str(payload)

    def _clear_radar(self) -> None:
        self.samples.clear()
        self.current_angle = None
        self.last_error = ""

    def _draw(self) -> None:
        width, height = self.screen.get_size()
        self.screen.fill((8, 12, 16))

        origin = (width // 2, height - 76)
        radius = min(width * 0.46, height * 0.78)

        self._draw_sector_background(origin, radius)
        self._draw_samples(origin, radius)
        self._draw_scan_line(origin, radius)
        self._draw_hud(width)

        pygame.display.flip()

    def _draw_sector_background(self, origin: tuple[int, int], radius: float) -> None:
        min_a, max_a = sorted((self.limits.min_angle, self.limits.max_angle))
        grid = (54, 78, 88)
        for frac in (0.25, 0.5, 0.75, 1.0):
            points = [origin]
            points.extend(self._arc_points(origin, radius * frac, min_a, max_a, 3))
            if len(points) > 2:
                pygame.draw.lines(self.screen, grid, False, points[1:], 1)

        for angle in range(((min_a + 14) // 15) * 15, max_a + 1, 15):
            end = self._point(origin, radius, angle)
            pygame.draw.line(self.screen, (36, 55, 64), origin, end, 1)
            label_pos = self._point(origin, radius + 18, angle)
            self._text(str(angle), label_pos, (105, 132, 144), self.small_font, center=True)

        outer = self._arc_points(origin, radius, min_a, max_a, 2)
        if len(outer) > 1:
            pygame.draw.lines(self.screen, (88, 128, 142), False, outer, 2)

    def _draw_samples(self, origin: tuple[int, int], radius: float) -> None:
        now = time.monotonic()
        angles = sorted(sample.angle for sample in self.samples.values())
        fallback_width = max(2.0, (self.limits.max_angle - self.limits.min_angle) / 64)
        overlay = pygame.Surface(self.screen.get_size(), pygame.SRCALPHA)

        for sample in self.samples.values():
            age = now - sample.received_at
            alpha = max(70, 190 - int(age * 20))
            half_width = self._half_width(sample.angle, angles, fallback_width)
            start_angle = sample.angle - half_width
            end_angle = sample.angle + half_width
            distance = max(self.limits.min_distance, min(self.limits.max_distance, sample.distance))

            visible_radius = self._distance_to_radius(distance, radius)
            min_radius = self._distance_to_radius(self.limits.min_distance, radius)
            max_radius = self._distance_to_radius(self.limits.max_distance, radius)

            if visible_radius > min_radius:
                visible = self._ring_sector(origin, min_radius, visible_radius, start_angle, end_angle)
                pygame.draw.polygon(overlay, (70, 205, 185, alpha), visible)

            if max_radius > visible_radius:
                hidden = self._ring_sector(origin, visible_radius, max_radius, start_angle, end_angle)
                pygame.draw.polygon(overlay, (16, 29, 38, min(230, alpha + 35)), hidden)

            hit = self._point(origin, visible_radius, sample.angle)
            pygame.draw.circle(overlay, (255, 224, 126, min(255, alpha + 40)), hit, 4)

        self.screen.blit(overlay, (0, 0))

    def _draw_scan_line(self, origin: tuple[int, int], radius: float) -> None:
        if self.current_angle is None:
            return
        end = self._point(origin, radius, self.current_angle)
        pygame.draw.line(self.screen, (95, 255, 214), origin, end, 3)
        pygame.draw.circle(self.screen, (95, 255, 214), origin, 5)

    def _draw_hud(self, width: int) -> None:
        lines = [
            f"port {self.port} @ {self.baud}",
            f"angle {self.limits.min_angle}-{self.limits.max_angle} deg",
            f"distance {self.limits.min_distance}-{self.limits.max_distance} mm",
            f"samples {len(self.samples)}",
            f"status {self.last_status}",
        ]
        if self.last_error:
            lines.append(f"serial error {self.last_error}")

        y = 16
        for line in lines:
            color = (235, 112, 112) if line.startswith("serial error") else (195, 218, 222)
            self._text(line, (18, y), color, self.font)
            y += 23

        self._text("visible", (width - 220, 18), (70, 205, 185), self.font)
        self._text("hidden behind hit", (width - 220, 42), (102, 122, 132), self.font)
        self._text("ESC/Q exits", (width - 220, 66), (160, 176, 184), self.font)

    def _distance_to_radius(self, distance: int, radius: float) -> float:
        span = max(1, self.limits.max_distance - self.limits.min_distance)
        return radius * max(0.0, min(1.0, (distance - self.limits.min_distance) / span))

    @staticmethod
    def _half_width(angle: int, angles: list[int], fallback: float) -> float:
        if len(angles) < 2:
            return fallback / 2
        index = angles.index(angle)
        left = angles[index] - angles[index - 1] if index > 0 else fallback
        right = angles[index + 1] - angles[index] if index + 1 < len(angles) else fallback
        return max(0.8, min(left, right, fallback) / 2)

    @staticmethod
    def _point(origin: tuple[int, int], radius: float, angle: float) -> tuple[int, int]:
        radians = math.radians(angle)
        x = origin[0] - math.cos(radians) * radius
        y = origin[1] - math.sin(radians) * radius
        return round(x), round(y)

    def _arc_points(
        self, origin: tuple[int, int], radius: float, start_angle: float, end_angle: float, step: float
    ) -> list[tuple[int, int]]:
        if end_angle < start_angle:
            start_angle, end_angle = end_angle, start_angle
        points = []
        angle = start_angle
        while angle <= end_angle:
            points.append(self._point(origin, radius, angle))
            angle += step
        points.append(self._point(origin, radius, end_angle))
        return points

    def _ring_sector(
        self,
        origin: tuple[int, int],
        inner_radius: float,
        outer_radius: float,
        start_angle: float,
        end_angle: float,
    ) -> list[tuple[int, int]]:
        outer = self._arc_points(origin, outer_radius, start_angle, end_angle, 1.5)
        inner = list(reversed(self._arc_points(origin, inner_radius, start_angle, end_angle, 1.5)))
        return outer + inner

    def _text(
        self,
        text: str,
        position: tuple[int, int],
        color: tuple[int, int, int],
        font: pygame.font.Font,
        center: bool = False,
    ) -> None:
        surface = font.render(text, True, color)
        rect = surface.get_rect()
        if center:
            rect.center = position
        else:
            rect.topleft = position
        self.screen.blit(surface, rect)


def available_ports() -> Iterable[str]:
    return (port.device for port in list_ports.comports())


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Visualize ultrasonic radar serial output.")
    parser.add_argument("port", nargs="?", help="Serial port, for example /dev/ttyACM0 or COM3")
    parser.add_argument("--baud", type=int, default=57600, help="Serial baud rate")
    parser.add_argument("--list-ports", action="store_true", help="Print serial ports and exit")
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)
    if args.list_ports:
        for port in available_ports():
            print(port)
        return 0

    if not args.port:
        ports = list(available_ports())
        if len(ports) == 1:
            args.port = ports[0]
        else:
            print("Serial port required. Use --list-ports to inspect available ports.", file=sys.stderr)
            return 2

    RadarView(args.port, args.baud).run()
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
