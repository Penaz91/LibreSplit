# Split Files

Split files are stores as well-formed JSON and **must** contain one main object.

You can use splits located in [the resource repository](https://github.com/LibreSplit/LibreSplit-resources/tree/main/splits) to start creating your own split files and place them however you want.

## Main Object

| Key             | Type               | Value                                   |
| --------------- | ------------------ | --------------------------------------- |
| `title`         | string             | Title string at top of window           |
| `attempt_count` | int                | Number of attempts                      |
| `start_delay`   | string (timestamp) | Non-negative delay until timer starts   |
| `world_record`  | string             | Best known time                         |
| `splits`        | array              | Array of [split objects](#split-object) |
| `theme`         | string             | Window theme                            |
| `theme_variant` | string             | Window theme variant                    |
| `width`         | int                | Window width                            |
| `height`        | int                | Window height                           |

Most of the above keys are optional.

## Split Object

| Key            | Type   | Value                  |
| -------------- | ------ | ---------------------- |
| `title`        | string | Split title            |
| `icon`         | string | Icon file path or url  |
| `time`         | string | Split time             |
| `best_time`    | string | Your best split time   |
| `best_segment` | string | Your best segment time |

Times are strings in `HH:MM:SS.mmmmmm` format.

Icons can be either a local file path (preferably absolute) or a URL. Note that only GTK-supported image formats will work. For example, `.svg` and `.webp` won't.

## Example

Here is a quick example of how a simple split file would look:

```json
{
    "title": "School - Homework%",
    "attempt_count": 55,
    "splits": [
        {
            "title": "Maths",
            "time": "05:12:55.123456",
            "best_time": "05:12:55.123456",
            "best_segment": "05:12:55.123456",
        },
        {
            "title": "Science",
            "time": "02:23:35.123456",
            "best_time": "01:15:35.789520",
            "best_segment": "01:15:35.789520",
        }
    ],
    "width": 250,
    "height": 500
}
```
