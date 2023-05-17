package ru.mrgrd56.wifithermometer.util;

import org.springframework.lang.NonNull;
import org.springframework.lang.Nullable;

import java.util.HashMap;
import java.util.Map;
import java.util.function.Supplier;

/**
 * Предоставляет интерфейс для создания мутабельной {@link Map}.<br>
 * При этом, ключи и значения могут быть {@code null}, в отличие от {@link Map#of} и {@link Map#ofEntries}.<br>
 * Создаваемая реализация {@link Map} может быть задана вручную, по умолчанию используется {@link HashMap}.
 * @param <K> Тип ключа.
 * @param <V> Тип значения.
 */
public class MapBuilder<K, V> {
    private final Map<K, V> map;

    /**
     * Создаёт экземпляр {@link MapBuilder}, используя реализацию {@link HashMap}.
     */
    public MapBuilder() {
        this(HashMap::new);
    }

    /**
     * Создаёт экземпляр {@link MapBuilder}, используя переданный {@code mapFactory} для создания {@link Map}.<br>
     * {@link Map}, создаваемая {@code mapFactory}, обязательно должна поддерживать операцию {@link Map#put}.
     * @param mapFactory Объект, создающий экземпляр {@link Map}
     */
    public MapBuilder(Supplier<Map<K, V>> mapFactory) {
        this.map = mapFactory.get();
    }

    /**
     * Помещает в создаваемую {@link Map} запись, где ключ - {@code key}, а значение - {@code value}.
     * @param key Ключ, с которым будет ассоциировано значение.
     * @param value Значение.
     */
    public MapBuilder<K, V> put(@Nullable K key, @Nullable V value) {
        map.put(key, value);
        return this;
    }

    /**
     * Помещает в создаваемую {@link Map} запись, где ключ - {@link Entry#key()},
     * а значение - {@link Entry#value()}.
     * @param entry Запись, добавляемая в создаваемую {@link Map}.
     */
    public MapBuilder<K, V> put(@NonNull Entry<K, V> entry) {
        return put(entry.key(), entry.value());
    }

    /**
     * Возвращает созданный экземпляр {@link Map}, реализация зависит от переданного
     * в конструктор {@link MapBuilder#MapBuilder(Supplier)} - {@code mapFactory}.
     */
    public Map<K, V> build() {
        return map;
    }

    /**
     * Создаёт экземпляр {@link Entry}.
     */
    public static <K, V> Entry<K, V> entry(@Nullable K key, @Nullable V value) {
        return new Entry<>(key, value);
    }

    /**
     * Создаёт экземпляр {@link HashMap}, помещая в неё переданные {@code entries}.
     */
    @SafeVarargs
    public static <K, V> Map<K, V> fromEntries(@NonNull Entry<K, V>... entries) {
        return fromEntriesInternal(new MapBuilder<>(), entries);
    }

    /**
     * Создаёт экземпляр {@link Map}, используя указанную {@code mapFactory}, помещая в неё переданные {@code entries}.
     */
    @SafeVarargs
    public static <K, V> Map<K, V> fromEntries(Supplier<Map<K, V>> mapFactory, @NonNull Entry<K, V>... entries) {
        return fromEntriesInternal(new MapBuilder<>(mapFactory), entries);
    }

    @SafeVarargs
    private static <K, V> Map<K, V> fromEntriesInternal(MapBuilder<K, V> builder, @NonNull Entry<K, V>... entries) {
        for (var entry : entries) {
            builder.put(entry);
        }
        return builder.build();
    }

    /**
     * Запись {@link Map}, использующаяся для создания {@link Map} с помощью {@link MapBuilder}. <br>
     * Несовместима с {@link Map.Entry}.
     * @param key
     * @param value
     * @param <K>
     * @param <V>
     */
    public record Entry<K, V>(
            @Nullable K key,
            @Nullable V value
    ) {}
}
