from __future__ import annotations

import argparse
import logging
from dataclasses import dataclass
from heapq import heappop, heappush
import typing as t

MapT = list[list[int]]

LOG = logging.getLogger(__name__)


@dataclass
class Pos:
    x: int
    y: int
    dist: int = -1
    previous: Pos | None = None

    def __lt__(self, other: object) -> bool:
        if not isinstance(other, Pos):
            raise NotImplementedError

        return self.dist < other.dist


MAP: MapT = [
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 1, 0, 0],
    [0, 0, 0, 1, 0, 0],
    [0, 0, 0, 1, 0, 0],
    [0, 0, 0, 1, 0, 0],
    [0, 0, 0, 1, 0, 0],
    [0, 0, 0, 1, 0, 0],
    [0, 0, 0, 1, 0, 0],
    [0, 0, 0, 0, 0, 0],
]

START = Pos(0, 0, 0)
END = Pos(5, 8)


def can_move(target: Pos, map: MapT) -> bool:
    y_size = len(map)
    x_size = len(map[0])
    if target.x < 0 or target.x >= x_size:
        return False
    if target.y < 0 or target.y >= y_size:
        return False
    if map[target.y][target.x] == 1:
        return False

    return True


def find_neighbours(cur_pos: Pos, map: MapT) -> t.Iterator[Pos]:
    cardinals = (
        # North
        Pos(cur_pos.x, cur_pos.y - 1, cur_pos.dist + 1, cur_pos),
        # South
        Pos(cur_pos.x, cur_pos.y + 1, cur_pos.dist + 1, cur_pos),
        # East
        Pos(cur_pos.x - 1, cur_pos.y, cur_pos.dist + 1, cur_pos),
        # West
        Pos(cur_pos.x + 1, cur_pos.y, cur_pos.dist + 1, cur_pos),
    )
    for c in cardinals:
        if not can_move(c, map):
            continue
        yield c


def print_map(map: MapT) -> None:
    total_traversible = sum(
        1 for y in range(len(map)) for x in range(len(map[y])) if map[y][x] == 0
    )
    LOG.debug(f"Traversible cells: {total_traversible}")

    print(f"   x: {', '.join(str(i) for i in range(len(map[0])))}")
    for x in range(len(MAP)):
        print(f"y: {x} {MAP[x]}")


def main() -> None:
    iter = 0

    to_visit = [START]
    visited: dict[tuple[int, int], Pos] = {}
    print_map(MAP)

    while to_visit:
        iter += 1
        if iter > 1_000:
            raise Exception("something fucked up...")

        next = heappop(to_visit)
        if next.x == END.x and next.y == END.y:
            print(f"Destination reached: {next}")
            visited[next.x, next.y] = next
            break

        LOG.debug(f"visiting {next}")
        for neighbour in find_neighbours(next, MAP):
            neighbour_pos = neighbour.x, neighbour.y
            if neighbour_pos not in visited:
                heappush(to_visit, neighbour)
            # Don't revisit neighbours if we've been there before for less distance.
            elif visited[neighbour_pos].dist > neighbour.dist:
                heappush(to_visit, neighbour)
            else:
                LOG.debug(
                    f"Skipping neighbour: {neighbour_pos} for distance {neighbour.dist}, already visited for dist: {visited[neighbour_pos].dist}"
                )

        pos = (next.x, next.y)
        if pos not in visited:
            LOG.debug(f"Storing entry for new pos {pos}")
            visited[pos] = next
            continue
        if next.dist < visited[pos].dist:
            LOG.info(
                f"Found new path to {pos} for dist {next.dist} (prev. {visited[pos].dist})"
            )
            visited[pos] = next
            continue
        else:
            LOG.debug(f"not updating {pos}, already found a shorter route.")

    end_pos = (END.x, END.y)
    if end_pos not in visited:
        print("Failed to resolve path!")
        print(f"total iter count: {iter}")
        return

    print_map(MAP)

    next = visited[end_pos]
    path = [next]
    print(path)
    while True:
        if next.previous:
            next = next.previous
            path.append(next)
        else:
            break

    path = path[::-1]

    for i in path:
        print(f"{i.x, i.y}")

    print(f"path length: {len(path)}")
    print(f"total iter count: {iter}")


if __name__ == "__main__":
    args = argparse.ArgumentParser()
    args.add_argument(
        "--log-level",
        default="INFO",
        choices=["DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"],
        help="Set the logging level"
    )
    parsed_args = args.parse_args()
    logging.basicConfig(level=getattr(logging, parsed_args.log_level))
    main()
