# Themes

LibreSplit can be customized using themes, which are made of CSS stylesheets.

## Changing the theme

You can set the global theme by changing the `theme` value in your `settings.json` configuration file, usually you'll find it in `~/.config/libresplit/`.

Also each split JSON file can apply their own themes by specifying a `theme` key in the main object, see [the split files documentation](split-files.md) for more information.

## Creating your own theme

1. Create a CSS stylesheet with your desired styles.
2. Place the stylesheet under the `~/.config/libresplit/themes/<name>/<name>.css`directory where `name` is the name of your theme. If you have your `XDG_CONFIG_HOME` env var pointing somewhere else, you may need to change the directory accordingly.
3. Theme variants should follow the pattern `<name>-<variant>.css`.

See the [GtkCssProvider documentation](https://docs.gtk.org/gtk3/css-properties.html) for a list of supported CSS properties. Note that you can also modify the default font-family.

| LibreSplit CSS classes        | Explanation Where needed                        |
| ----------------------------- | ----------------------------------------------- |
| `.window`                     |                                                 |
| `.header`                     |                                                 |
| `.title`                      |                                                 |
| `.attempt-count`              |                                                 |
| `.time`                       |                                                 |
| `.delta`                      |                                                 |
| `.time`                       |                                                 |
| `.timer`                      |                                                 |
| `.timer-seconds`              |                                                 |
| `.timer-millis`               |                                                 |
| `.delay`                      | Timer not running/in negative time              |
| `.splits`                     | Container of the splits                         |
| `.split`                      | The splits themselves                           |
| `.current-split`              |                                                 |
| `.split-title`                |                                                 |
| `.split-icon`                 |                                                 |
| `.split-time`                 |                                                 |
| `.split-delta`                | Comparison time in the split                    |
| `.split-last`                 | The last split, if its not yet scrolled down to |
| `.done`                       |                                                 |
| `.behind`                     | Behind the PB but gaining time                  |
| `.losing`                     | Ahead of PB but losing time                     |
| `.behind.losing`              | (class combination) Behind PB and losing time   |
| `.best-segment`               |                                                 |
| `.best-split`                 |                                                 |
| `.footer`                     |                                                 |
| `.prev-segment-label`         |                                                 |
| `.prev-segment`               |                                                 |
| `.segment`                    |                                                 |
| `.segment-best`               |                                                 |
| `.segment-pb`                 |                                                 |
| `.segment-seconds`            |                                                 |
| `.segment-millis`             |                                                 |
| `.sum-of-bests-label`         |                                                 |
| `.sum-of-bests`               |                                                 |
| `.split-icon`                 |                                                 |
| `.personal-best-label`        |                                                 |
| `.personal-best`              |                                                 |
| `.world-record-label`         |                                                 |
| `.world-record`               |                                                 |

If a split has a `title` key, its UI element receives a class name derived from its title.

Specifically, the title is lowercase and all non-alphanumeric characters are replaced with hyphens, and the result is concatenated with `split-title-`.

For instance, if your split is titled "First split", it can be styled by targeting the CSS class `.split-title-first-split`.

## FAQ

### How do I hide a section of LibreSplit?

GTK does not have a built-in way of hiding pieces of the interface, but you can hide most items by setting the font-size to zero. For instance:

```css
.segment-pb, .segment-best{
    font-size: 0;
}
```

Will hide the PB and Best sections.

### Is there anything to help me develop themes out there?

In fact, there is!

If you run LibreSplit from a terminal like this:

```sh
GTK_DEBUG=interactive libresplit
```

LiveSplit will be started together with another window: the interactive GTK debugger. Like the one you see below:

![The GTK Debug window](./images/gtk_debugger.png)

Make sure that both LibreSplit and this window are visible, because when you click on one row in the GTK debugger (`Objects` tab), the corresponding section in LiveSplit will flash 3 times, letting you know what you selected.

Once you found what you want to edit, take a note of its `style class` (See [Creating your own theme](#creating-your-own-theme) for a list) and head to the `CSS` tab: there you can edit in real time LiveSplit's aspect. These edits are temporary, but they can help you developing your own CSS theme.

Once you're done developing your theme, feel free to share it with the community!
